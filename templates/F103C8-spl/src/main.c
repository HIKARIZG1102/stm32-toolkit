/**
 * Minimal STM32 project template.
 *
 * Hardware configuration is in inc/board.h — edit it to match your board.
 * Common board definitions are available in the boards/ directory
 * of the stm32-toolkit repository.
 */

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "board.h"

extern uint32_t _estack;

int main(void);
void SystemInit(void);

__attribute__((used, section(".isr_vector")))
void (*const v[])(void) = {
    (void (*)())&_estack,
    (void (*)())main,
};

static void delay(volatile uint32_t c) { while (c--); }

int main(void) {
    SystemInit();

    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = LED_GPIO_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_GPIO_PORT, &gpio);

    while (1) {
        GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
        delay(2000000);
        GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
        delay(2000000);
    }
}
