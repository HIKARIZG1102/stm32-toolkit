/* __PROJECT_NAME__ - STM32 HAL Template */
/* 注意: 本 HAL 库的 HAL_Init() 不可用（内部死循环），请用寄存器配引脚 */
#include "stm32f1xx_hal.h"

#define SP 0x2000C000

void main(void);

__attribute__((used, section(".isr_vector")))
void(*const v[])(void) = {
    (void(*)())SP, main
};

static void delay(volatile uint32_t c) { while (c--); }

void main(void) {
    /* 示例: PB13 = 输出, PB14 = 上拉输入 */
    /* 用户根据实际引脚修改 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH = (GPIOB->CRH & ~0xFF000000) | 0x08200000;
    GPIOB->ODR |= (1 << 14);

    /* HAL GPIO 读写函数正常可用 */
    /* HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); */
    /* HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14); */
    /* HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13); */

    while (1) {}
}
