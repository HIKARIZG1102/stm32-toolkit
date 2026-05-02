/* __PROJECT_NAME__ - STM32 HAL Template */
/* 注意事项:
 *   - 本 HAL 库的 HAL_Init() 不可用（SysTick_Config/NVIC_SetPriority 有 bug）
 *   - GPIO 用寄存器配置（HAL_GPIO_Init 写 CRH 也有 bug）
 *   - HAL_GPIO_ReadPin / WritePin / TogglePin 正常可用
 */
#include "stm32f1xx_hal.h"

#define SP 0x20005000

void main(void);

__attribute__((used, section(".isr_vector")))
void(*const v[])(void) = {
    (void(*)())SP, main
};

static void delay(volatile uint32_t c) { while (c--); }

void main(void) {
    /* 用户在此添加自己的初始化代码 */
    while (1) {}
}
