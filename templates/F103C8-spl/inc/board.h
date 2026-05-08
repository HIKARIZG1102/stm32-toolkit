#ifndef __BOARD_H
#define __BOARD_H

/**
 * Board-specific hardware definitions.
 *
 * Edit this file to match your actual PCB.
 * Common configurations are available in boards/ directory.
 */

#include "stm32f10x.h"

/* ── LED (Blue Pill: PC13) ── */
#define LED_GPIO_PORT       GPIOC
#define LED_GPIO_PIN        GPIO_Pin_13
#define LED_GPIO_CLK        RCC_APB2Periph_GPIOC

/* ── USART (debug printf) ── */
#define USARTx              USART1
#define USART_TX_PIN        GPIO_Pin_9
#define USART_TX_PORT       GPIOA
#define USART_RX_PIN        GPIO_Pin_10
#define USART_RX_PORT       GPIOA
#define USART_GPIO_CLK      RCC_APB2Periph_GPIOA
#define USART_CLK           RCC_APB2Periph_USART1
#define USART_BAUDRATE      115200

/* ── External crystal (HSE) ── */
#define HSE_FREQ_HZ         8000000

#endif /* __BOARD_H */
