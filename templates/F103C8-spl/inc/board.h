#ifndef __BOARD_H
#define __BOARD_H

/**
 * @brief 板级硬件配置 — F103C8（Blue Pill 及兼容板）
 *
 * 编辑此文件以匹配你的实际 PCB。
 * 标准板定义见 stm32-toolkit/boards/ 目录。
 */

#include "stm32f10x.h"


/* ═══════════════════════════════════════════════════════════════
 *  LED 配置
 *
 *  Blue Pill (F103C8):  GPIOC, Pin13 (PC13, 低电平亮)
 *  其他板子请根据原理图修改。
 * ═══════════════════════════════════════════════════════════════ */
#define LED_GPIO_PORT       GPIOC
#define LED_GPIO_PIN        GPIO_Pin_13
#define LED_GPIO_CLK        RCC_APB2Periph_GPIOC


/* ═══════════════════════════════════════════════════════════════
 *  USART 配置
 *
 *  默认 USART1 (PA9 TX, PA10 RX)，115200-8N1。
 *  如需换到 USART2/3，同时修改下方 5 个宏。
 * ═══════════════════════════════════════════════════════════════ */
#define USARTx              USART1
#define USART_TX_PIN        GPIO_Pin_9
#define USART_TX_PORT       GPIOA
#define USART_RX_PIN        GPIO_Pin_10
#define USART_RX_PORT       GPIOA
#define USART_GPIO_CLK      RCC_APB2Periph_GPIOA
#define USART_CLK           RCC_APB2Periph_USART1
#define USART_BAUDRATE      115200


/* ═══════════════════════════════════════════════════════════════
 *  外部晶振频率
 *
 *  大部分 Blue Pill 使用 8MHz 晶振。
 *  如果你的板子没有晶振或使用 12MHz，请修改此值。
 * ═══════════════════════════════════════════════════════════════ */
#define HSE_FREQ_HZ         8000000


/* ========== USER CODE BEGIN ========== */
/* ========== USER CODE END ========== */

#endif /* __BOARD_H */
