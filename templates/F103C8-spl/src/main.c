/**
 * @file    main.c
 * @brief   STM32 项目模板 — 用户应用程序入口
 *
 * ═══════════════════════════════════════════════════════════════
 *  快速上手
 * ═══════════════════════════════════════════════════════════════
 *
 * 1. 编辑 inc/board.h — 配置板子引脚
 * 2. 编译: stm32make build
 * 3. 烧录: stm32make flash
 *
 * ── 需要中断？ ──
 * 启动文件 src/startup.c 已提供完整向量表。
 * 直接定义对应的 IRQHandler 即可:
 *
 *     void TIM2_IRQHandler(void) { ... }
 *
 * ── 需要 printf？ ──
 * 取消下方 USART 区域注释。
 *
 * ── 时钟 ──
 * 默认 8MHz 晶振 → PLL → 72MHz。
 * 如无晶振或频率不同，见 main.c 顶部或 board.h 注释。
 *
 * ═══════════════════════════════════════════════════════════════
 */

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "board.h"

/* ========== USER CODE BEGIN (头文件) ========== */
/* ========== USER CODE END ========== */


/* ========== USER CODE BEGIN (全局变量) ========== */
/* ========== USER CODE END ========== */


/* ── USART / printf 配置（启用: #if 0 → #if 1） ── */
#if 0
#include "stm32f10x_usart.h"
#include <stdio.h>

static void usart_init(void) {
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;
    RCC_APB2PeriphClockCmd(USART_GPIO_CLK | USART_CLK, ENABLE);
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = USART_TX_PIN; gpio.GPIO_Speed = GPIO_Speed_50MHz; gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(USART_TX_PORT, &gpio);
    gpio.GPIO_Pin = USART_RX_PIN; gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_RX_PORT, &gpio);
    USART_StructInit(&usart);
    usart.USART_BaudRate = USART_BAUDRATE;
    usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USARTx, &usart);
    USART_Cmd(USARTx, ENABLE);
}

int _write(int file, char *ptr, int len) {
    (void)file;
    for (int i = 0; i < len; i++) {
        while (!(USARTx->SR & USART_SR_TXE));
        USARTx->DR = (uint8_t)ptr[i];
    }
    return len;
}
#endif


/* ── 简单延时 ── */
static void delay(volatile uint32_t c) { while (c--); }


int main(void)
{
    /* ========== USER CODE BEGIN (初始化) ========== */

    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = LED_GPIO_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_GPIO_PORT, &gpio);

    /* usart_init(); */
    /* printf("STM32 started\r\n"); */

    /* ========== USER CODE END ========== */


    /* ========== USER CODE BEGIN (主循环) ========== */

    while (1)
    {
        GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
        delay(2000000);
        GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
        delay(2000000);
    }

    /* ========== USER CODE END ========== */
}


/* ========== USER CODE BEGIN (其他函数) ========== */
/* ========== USER CODE END ========== */
