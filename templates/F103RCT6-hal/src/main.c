/* main.c — STM32 HAL 项目模板
 *
 * 已知问题（gcc 工具链）：
 * 1. VTOR 要求 512 字节对齐 —— 如果使用 RAM 向量表，必须加 aligned(512)
 * 2. SystemCoreClock 须在 HAL_Init() 前设置（内部 HSI=8MHz，默认值 72MHz）
 * 3. CRH 寄存器操作使用 (reg & ~(0xF << bit)) | (val << bit) 方式，避免掩码错误
 */
#include "stm32f1xx_hal.h"

/* RAM 向量表（如需 HAL_Init + SysTick 中断，必须 512 字节对齐） */
static uint32_t ram_vector[256] __attribute__((aligned(512)));

void SysTick_Handler(void) { HAL_IncTick(); }

static void ram_vector_init(void) {
    uint32_t *flash_vtor = (uint32_t *)0x08000000;
    for (int i = 0; i < 256; i++)
        ram_vector[i] = flash_vtor[i];
    ram_vector[15] = (uint32_t)SysTick_Handler | 1;
    SCB->VTOR = (uint32_t)ram_vector;
}

int main(void) {
    /* 1. 切到 RAM 向量表（如需 SysTick 中断） */
    ram_vector_init();

    /* 2. 开 GPIOB 时钟 + 配引脚 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    /* PB13 = 推挽输出 50MHz, PB14 = 输入上拉 */
    GPIOB->CRH = (GPIOB->CRH & ~0xFFF00000) | 0x08300000;
    GPIOB->ODR |= (1 << 14);

    /* 3. 设 SystemCoreClock（必须在 HAL_Init 之前！） */
    SystemCoreClock = 8000000;

    /* 4. HAL 初始化 */
    HAL_Init();

    /* --- 以下为示例代码 --- */

    /* 闪 5 次验证 */
    for (int i = 0; i < 5; i++) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
        HAL_Delay(300);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
        HAL_Delay(300);
    }

    /* 按键翻转（PB14 按下 = 低电平） */
    uint8_t last = 1;
    while (1) {
        uint8_t key = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);
        if (key == 0 && last == 1)
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
        last = key;
        HAL_Delay(20);
    }
}
