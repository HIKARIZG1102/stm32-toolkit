/* __PROJECT_NAME__ - STM32 HAL Template */
/* 注意: 本 HAL 库的 HAL_Init() 不可用（内部死循环），请用寄存器配引脚 */
#include "stm32f1xx_hal.h"

#define SP 0x20005000

void main(void);

__attribute__((used, section(".isr_vector")))
void(*const v[])(void) = {
    (void(*)())SP, main
};

static void delay(volatile uint32_t c) { while (c--); }

void main(void) {
    /* 示例: PC13 = 输出, PA0 = 上拉输入 */
    /* 用户根据实际引脚修改 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH = (GPIOC->CRH & ~0x00F00000) | 0x00200000;

    /* HAL GPIO 读写函数正常可用 */
    /* HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); */
    /* HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0); */
    /* HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); */

    while (1) {}
}
