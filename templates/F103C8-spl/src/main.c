/**
 * STM32 project template (SPL/bare-metal mode).
 *
 * Hardware pinout is configured in inc/board.h.
 * Edit board.h to match your PCB — no code changes in main.c needed.
 */

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "board.h"

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

void Reset_Handler(void);
void SystemInit(void);
int main(void);

__attribute__((used, section(".isr_vector")))
void (*const v[])(void) = {
    (void (*)())&_estack,
    Reset_Handler,
};

void __attribute__((naked)) Reset_Handler(void) {
    uint32_t *src, *dst;
    for (src = &_sidata, dst = &_sdata; dst < &_edata; src++, dst++)
        *dst = *src;
    for (dst = &_sbss; dst < &_ebss; dst++)
        *dst = 0;
    SystemInit();
    main();
    while (1);
}

static void delay(volatile uint32_t c) { while (c--); }

int main(void) {
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
