/**
 * @file    main.c
 * @brief   STM32 项目模板 — 用户应用程序入口
 *
 * ═══════════════════════════════════════════════════════════════
 *  快速上手
 * ═══════════════════════════════════════════════════════════════
 *
 * 1. 编辑 inc/board.h — 配置你的板子引脚（LED、串口等）
 * 2. 编辑 inc/stm32f10x_conf.h — 开启/关闭外设模块
 * 3. 编译: stm32make build
 * 4. 烧录: stm32make flash
 *
 * ── 需要中断？ ──
 * 启动文件 src/startup.c 已提供完整向量表（82 项）。
 * 直接在 main.c（或新建文件）里定义对应的 IRQHandler:
 *
 *     void TIM2_IRQHandler(void) {
 *         if (TIM_GetITStatus(TIM2, TIM_IT_Update)) {
 *             TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
 *             // 用户代码
 *         }
 *     }
 *
 * ── 需要 printf？ ──
 * 取消下方 "USER CODE: USART / printf" 区域的注释即可。
 * 串口引脚在 board.h 中配置。
 *
 * ═══════════════════════════════════════════════════════════════
 *  时钟配置说明（详见 system_stm32f10x.c / stm32f10x.h）
 * ═══════════════════════════════════════════════════════════════
 *
 * 默认配置（SystemInit）:
 *   时钟源: HSE（外部晶振）→ PLL → SYSCLK = 72MHz
 *   HSE 频率: 8 MHz（定义在 stm32f10x.h 的 HSE_VALUE）
 *   AHB = 72MHz, APB1 = 36MHz, APB2 = 72MHz
 *
 * 如果你的板子:
 *   ✅ 有 8MHz 晶振 → 默认配置，无需改动
 *   ✅ 有 12MHz 晶振 → 修改 stm32f10x.h 中 HSE_VALUE 为 12000000
 *   ❌ 没有晶振      → 见下方"无外部晶振"注释
 *
 * ── 无外部晶振（使用 HSI 8MHz）─
 * 编辑 system_stm32f10x.c，找到 SetSysClockTo72() 函数，
 * 将 HSE 相关代码替换为 HSI 配置（或直接注释 SystemInit 调用，
 * 系统默认使用 HSI 8MHz 运行）。
 *
 * ═══════════════════════════════════════════════════════════════
 */

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "board.h"

/* ========== USER CODE BEGIN (包含头文件) ========== */
/* 在这里添加你需要的头文件 */
/* #include "stm32f10x_tim.h" */
/* #include "stm32f10x_usart.h" */
/* #include <stdio.h> */
/* ========== USER CODE END ========== */


/* ========== USER CODE BEGIN (全局变量) ========== */


/* ========== USER CODE END ========== */


/* ── USART / printf 配置区域 ───────────────────────
 * 启用方法: 将下方的 #if 0 改为 #if 1
 * 串口引脚在 inc/board.h 的 USART_xxx 宏中配置。
 * ──────────────────────────────────────────────────── */
#if 0
#include "stm32f10x_usart.h"
#include <stdio.h>

/* 串口初始化（引脚配置见 board.h） */
static void usart_init(void) {
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

    RCC_APB2PeriphClockCmd(USART_GPIO_CLK | USART_CLK, ENABLE);

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = USART_TX_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(USART_TX_PORT, &gpio);

    gpio.GPIO_Pin   = USART_RX_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_RX_PORT, &gpio);

    USART_StructInit(&usart);
    usart.USART_BaudRate = USART_BAUDRATE;
    usart.USART_Mode     = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USARTx, &usart);
    USART_Cmd(USARTx, ENABLE);
}

/* printf 重定向 → USART
 * 注意: startup.c 已初始化 .data/.bss，printf 内部变量可正常使用 */
int _write(int file, char *ptr, int len) {
    (void)file;
    for (int i = 0; i < len; i++) {
        while (!(USARTx->SR & USART_SR_TXE));
        USARTx->DR = (uint8_t)ptr[i];
    }
    return len;
}
#endif

/* ═══════════════════════════════════════════════════════════════
 *  简单延时（适用于初期测试，精度不高）
 *  72MHz 下: delay(1000000) ≈ 30ms
 * ═══════════════════════════════════════════════════════════════ */
static void delay(volatile uint32_t c) { while (c--); }


/* ═══════════════════════════════════════════════════════════════
 *  main() — 用户主函数
 *
 *  注意: main() 由 startup.c 中的 Reset_Handler 调用，
 *  此时 .data/.bss 已初始化，SystemInit() 已完成时钟配置。
 *  直接编写你的应用代码即可。
 * ═══════════════════════════════════════════════════════════════ */
int main(void)
{
    /* ========== USER CODE BEGIN (初始化) ========== */

    /* ── LED 初始化（引脚配置见 board.h） ── */
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = LED_GPIO_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_GPIO_PORT, &gpio);

    /* ── 串口初始化（取消上方 USART 区域注释后启用） ── */
    /* usart_init(); */
    /* printf("\r\nSTM32 启动成功\r\n"); */

    /* ========== USER CODE END ========== */


    /* ========== USER CODE BEGIN (主循环) ========== */

    while (1)
    {
        /* LED 闪烁：验证程序运行 */
        GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);  /* 亮 */
        delay(2000000);
        GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);    /* 灭 */
        delay(2000000);
    }

    /* ========== USER CODE END ========== */
}


/* ========== USER CODE BEGIN (其他函数) ========== */
/* 在这里添加你的中断处理、功能函数等 */
/* ========== USER CODE END ========== */
