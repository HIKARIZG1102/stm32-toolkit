/* main.c — STM32 HAL 项目模板
 *
 * 注意事项：
 * 1. VTOR 要求 512 字节对齐 —— ram_vector 必须 __attribute__((aligned(512)))
 * 2. SystemCoreClock 须在 HAL_Init() 前设置
 * 3. CRH 寄存器操作使用 (reg & ~(0xF << bit)) | (val << bit) 方式
 * 4. printf 已重定向到 USART1 (PA9=TX, PA10=RX) @ 115200
 * 5. 全局变量 .data 初始化需要 link.ld 中 _sidata 4 字节对齐
 */
#include "stm32f1xx_hal.h"
#include <stdio.h>

/* RAM 向量表（HAL_Init + SysTick 中断需要 512 字节对齐） */
static uint32_t ram_vector[256] __attribute__((aligned(512)));

void SysTick_Handler(void) { HAL_IncTick(); }

static void ram_vector_init(void) {
    uint32_t *flash_vtor = (uint32_t *)0x08000000;
    for (int i = 0; i < 256; i++)
        ram_vector[i] = flash_vtor[i];
    ram_vector[15] = (uint32_t)SysTick_Handler | 1;
    SCB->VTOR = (uint32_t)ram_vector;
}

/* ============ printf → USART1 (寄存器级) ============ */

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        while ((USART1->SR & USART_SR_TC) == 0);
        USART1->DR = (uint8_t)ptr[i];
    }
    return len;
}

static void MX_USART1_UART_Init(void) {
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_9;  gpio.Mode = GPIO_MODE_AF_PP;  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio);
    gpio.Pin = GPIO_PIN_10; gpio.Mode = GPIO_MODE_INPUT;  gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);

    UART_HandleTypeDef huart1 = {0};
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);
}

int main(void) {
    /* 1. 切到 RAM 向量表（SysTick 中断） */
    ram_vector_init();

    /* 2. 设 SystemCoreClock（HAL_Init 之前! HSI=8MHz） */
    SystemCoreClock = 8000000;

    /* 3. HAL 初始化 */
    HAL_Init();

    /* 4. 串口 115200 */
    MX_USART1_UART_Init();

    /* 5. LED (PB13) + KEY (PB14) */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH = (GPIOB->CRH & ~0xFFF00000) | 0x08300000;
    GPIOB->ODR |= (1 << 14);

    /* 6. 测试 */
    printf("\r\nSTM32 HAL template OK\r\n");

    uint8_t last = 1;
    while (1) {
        uint8_t key = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);
        if (key == 0 && last == 1) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
            printf("KEY pressed\r\n");
        }
        last = key;
        HAL_Delay(20);
    }
}
