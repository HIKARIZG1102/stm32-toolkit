/**
 * board/mini_stm32_rct6.h — Board definition for MiniSTM32 F103RCT6
 *
 * Common board: MiniSTM32 (正点原子 Mini 版), STM32F103RCT6
 * LED: PB13 (active low)
 * USART1: PA9(TX), PA10(RX)
 * HSE: 8MHz external crystal
 */

#ifndef BOARD_MINI_STM32_RCT6_H
#define BOARD_MINI_STM32_RCT6_H

/* ── LED ── */
#define LED_GPIO_PORT       GPIOB
#define LED_GPIO_PIN        GPIO_Pin_13
#define LED_GPIO_CLK        RCC_APB2Periph_GPIOB

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

#endif /* BOARD_MINI_STM32_RCT6_H */
