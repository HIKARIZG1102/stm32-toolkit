#include "stm32f1xx_hal.h"

#define SP 0x20005000

void main(void);
void SysTick_Handler(void) { HAL_IncTick(); }

__attribute__((used, section(".isr_vector")))
void(*const v[])(void) = {
    (void(*)())SP, main, 0,0,0,0,0,0,0,0,0,0,0,0,0, SysTick_Handler
};

void main(void) {
    HAL_Init();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_13;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio);
    while (1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(500);
    }
}
