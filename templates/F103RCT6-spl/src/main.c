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

/* Stack and section symbols from linker script */
extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

void Reset_Handler(void);
void SystemInit(void);
int main(void);

/* ── Vector table (minimal: SP + Reset only) ──
 * For interrupt-driven projects, replace this with a full vector table
 * (see examples/TIM_General/src/startup.c for reference). */
__attribute__((used, section(".isr_vector")))
void (*const v[])(void) = {
    (void (*)())&_estack,
    Reset_Handler,
};

/* ── Reset handler: init .data/.bss, setup clocks, enter main ── */
void __attribute__((naked)) Reset_Handler(void) {
    uint32_t *src, *dst;

    /* Copy .data from flash to RAM */
    for (src = &_sidata, dst = &_sdata; dst < &_edata; src++, dst++)
        *dst = *src;

    /* Zero .bss */
    for (dst = &_sbss; dst < &_ebss; dst++)
        *dst = 0;

    SystemInit();
    main();
    while (1);
}

/* ── Simple delay ── */
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
