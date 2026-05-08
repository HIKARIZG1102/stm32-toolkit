/**
 * boards/bluepill.h — Board definition for STM32F103C8 "Blue Pill"
 *
 * LED: PC13 (active low, onboard)
 * USART1: PA9(TX), PA10(RX)
 * HSE: 8MHz external crystal (most Blue Pill boards)
 *
 * NOTE: Some Blue Pill clones use a 12MHz or no external crystal.
 * If your board has no HSE, the system will fall back to HSI (8MHz).
 */

#ifndef BOARD_BLUEPILL_H
#define BOARD_BLUEPILL_H

/* ── LED ── */
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

/* ── External crystal ── */
#define HSE_FREQ_HZ         8000000

#endif /* BOARD_BLUEPILL_H */
