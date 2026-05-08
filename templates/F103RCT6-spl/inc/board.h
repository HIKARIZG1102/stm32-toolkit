#ifndef __BOARD_H
#define __BOARD_H

/**
 * @brief 板级硬件配置
 *
 * 编辑此文件以匹配你的实际 PCB。
 * 标准板定义见 stm32-toolkit/boards/ 目录。
 *
 * 使用方法:
 *   1. 修改下方 LED_xxx / USART_xxx 宏为你的板子实际引脚
 *   2. 确认 HSE_FREQ_HZ 与实际晶振频率一致
 *   3. 编译烧录即可
 *
 * 移植到不同板子:
 *   只需修改此文件，main.c 无需改动。
 */

#include "stm32f10x.h"


/* ═══════════════════════════════════════════════════════════════
 *  LED 配置
 *
 *  指向板上的 LED 引脚。
 *  如果板上没有 LED，注释掉即可。
 *
 *  常见接法:
 *    MiniSTM32 RCT6:  GPIOB, Pin13   (PB13)
 *    Blue Pill C8:     GPIOC, Pin13  (PC13)
 *    正点原子 Mini:    GPIOB, Pin13  (PB13)
 * ═══════════════════════════════════════════════════════════════ */
#define LED_GPIO_PORT       GPIOB
#define LED_GPIO_PIN        GPIO_Pin_13
#define LED_GPIO_CLK        RCC_APB2Periph_GPIOB


/* ═══════════════════════════════════════════════════════════════
 *  USART / 串口调试 配置
 *
 *  用 USB 转串口模块（CH340/CP2102）连接至这些引脚。
 *  启用方法: 取消 main.c 中 USART 区域注释。
 *
 *  常见接法:
 *    USART1: PA9(TX), PA10(RX)  ← 最常用
 *    USART2: PA2(TX), PA3(RX)
 *    USART3: PB10(TX), PB11(RX)
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
 *  外部晶振 (HSE) 频率
 *
 *  SystemInit() 使用此频率通过 PLL 倍频至 72MHz。
 *  默认值适用于大多数开发板。
 *
 *  常见晶振:
 *    8 MHz  ← 最常见（Blue Pill, MiniSTM32, 正点原子等）
 *   12 MHz  ← 部分板子使用
 *  (无)    ← 使用内部 HSI 8MHz，见 main.c 顶部注释
 * ═══════════════════════════════════════════════════════════════ */
#define HSE_FREQ_HZ         8000000


/* ========== USER CODE BEGIN (附加板级配置) ========== */
/* 在这里添加你的板级特定宏，例如:
 *   #define BTN_GPIO_PORT   GPIOA
 *   #define BTN_GPIO_PIN    GPIO_Pin_0
 */
/* ========== USER CODE END ========== */

#endif /* __BOARD_H */
