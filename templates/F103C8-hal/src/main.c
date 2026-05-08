/* main.c — STM32 HAL 项目模板
 *
 * 注意事项：
 * 1. VTOR 要求 512 字节对齐 —— ram_vector 必须 __attribute__((aligned(512)))
 * 2. SystemCoreClock 须在 HAL_Init() 前设置
 * 3. CRH 寄存器操作使用 (reg & ~(0xF << bit)) | (val << bit) 模式
 * 4. printf 已重定向到 USART1，可直接用 printf 打印
 *
 * 板载外设：
 *   PB13 = LED (高电平灭 / 低电平亮)
 *   PB14 = KEY (低有效, 按下 = GND)
 *   PA9  = USART1_TX
 *   PA10 = USART1_RX
 */

#include "stm32f1xx_hal.h"

/* RAM 向量表（HAL_Init + SysTick 中断时需 512 字节对齐） */
static uint32_t ram_vector[256] __attribute__((aligned(512)));

void SysTick_Handler(void) { HAL_IncTick(); }

static void ram_vector_init(void)
{
    uint32_t *flash_vtor = (uint32_t *)0x08000000;
    for (int i = 0; i < 256; i++)
        ram_vector[i] = flash_vtor[i];
    ram_vector[15] = (uint32_t)SysTick_Handler | 1;
    SCB->VTOR = (uint32_t)ram_vector;
}

/* ---- UART1 初始化（PA9 TX, PA10 RX） ---- */
static void MX_USART1_UART_Init(void)
{
    UART_HandleTypeDef huart1;

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    /* PA9(TX) = AF推挽输出 50MHz, PA10(RX) = 浮空输入 */
    GPIOA->CRH = (GPIOA->CRH & ~0xFF0) | 0x4B0;

    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);
}

/* ---- printf 重定向到 USART1 ---- */
int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++) {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = (uint8_t)ptr[i];
    }
    while (!(USART1->SR & USART_SR_TC));
    return len;
}

/* ---- 板级初始化（GPIO + UART） ---- */
static void MX_GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    /* PB13(LED) = 推挽输出 50MHz, PB14(KEY) = 上拉输入 */
    GPIOB->CRH = (GPIOB->CRH & ~0xFFF00000) | 0x08300000;
    GPIOB->ODR |= (1 << 14); /* KEY 上拉 */
}

int main(void)
{
    ram_vector_init();
    MX_GPIO_Init();

    SystemCoreClock = 8000000;
    HAL_Init();
    MX_USART1_UART_Init();

    printf("\n========== STM32F103C8 HAL Template ==========\n");
    printf("PB13 LED / PB14 KEY / USART1 115200\n\n");

    /* 开机闪 5 下 */
    for (int i = 0; i < 5; i++) {
        GPIOB->ODR &= ~(1 << 13);  /* LED 亮 */
        HAL_Delay(200);
        GPIOB->ODR |= (1 << 13);   /* LED 灭 */
        HAL_Delay(200);
    }

    printf("Press KEY (PB14) to toggle LED\n");

    uint8_t last = 1;
    while (1) {
        uint8_t key = !!(GPIOB->IDR & (1 << 14));
        if (key == 0 && last == 1) {
            GPIOB->ODR ^= (1 << 13);
            printf("LED toggled\n");
        }
        last = key;
        HAL_Delay(50);
    }
}
