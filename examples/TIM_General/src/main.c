/* main.c — TIM_General：通用定时器 TIM2 定时（1ms 中断，1s LED 闪烁）
 * 复现自：7-1-TIMx-通用定时器-定时code2
 * 设计模式：ISR 只做计时（time++），主循环轮询变量
 * 芯片：STM32F103RCT6（HD），SPL 库
 */

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include <stdio.h>

/* ═══════════════════════ 硬件引脚定义 ═══════════════════════ */
#define LED_GPIO_PORT       GPIOB
#define LED_GPIO_PIN        GPIO_Pin_13
#define LED_GPIO_CLK        RCC_APB2Periph_GPIOB

/* ═══════════════════════ 全局变量 ═══════════════════════ */
volatile uint32_t timer_ms = 0;   /* 1ms 递增的计时器（ISR 中更新） */

/* ═══════════════════════ USART1 初始化 ═══════════════════════ */
/* PA9 — TX, PA10 — RX, 115200-8N1 */
static void USART1_Config(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* TX: PA9, 推挽复用输出 */
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);

    /* RX: PA10, 浮空输入 */
    gpio.GPIO_Pin   = GPIO_Pin_10;
    gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    USART_StructInit(&usart);
    usart.USART_BaudRate = 115200;
    usart.USART_Mode     = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}

/* 重定向 printf 到 USART1（覆盖 nosys 的弱符号 _write） */
int _write(int file, char *ptr, int len)
{
    (void)file;
    for (int i = 0; i < len; i++) {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = (uint8_t)ptr[i];
    }
    return len;
}

/* ═══════════════════════ LED 初始化 ═══════════════════════ */
static void LED_Config(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = LED_GPIO_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_GPIO_PORT, &gpio);
    GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);  /* 初始熄灭 */
}

/* ═══════════════════════ TIM2 初始化（通用定时器） ═══════════════════════ */
/* 时钟：APB1 = 72MHz
 * Prescaler = 72-1  => 72MHz / 72 = 1MHz
 * Period = 1000-1   => 1MHz / 1000 = 1kHz => 1ms 中断
 */
static void TIM2_Config(void)
{
    TIM_TimeBaseInitTypeDef tim;
    NVIC_InitTypeDef nvic;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructInit(&tim);
    tim.TIM_Prescaler         = 72 - 1;
    tim.TIM_Period            = 1000 - 1;
    tim.TIM_ClockDivision     = TIM_CKD_DIV1;
    tim.TIM_CounterMode       = TIM_CounterMode_Up;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &tim);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);

    /* NVIC 配置 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic.NVIC_IRQChannel                   = TIM2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority        = 3;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);
}

/* ═══════════════════════ TIM2 中断处理 ═══════════════════════ */
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        timer_ms++;                     /* 1ms 递增 */
    }
}

/* ═══════════════════════ 主函数 ═══════════════════════ */
int main(void)
{
    uint32_t last_toggle = 0;

    LED_Config();
    USART1_Config();
    TIM2_Config();

    printf("\r\n========== TIM_General (TIM2 通用定时器) ==========\r\n");
    printf("TIM2 Prescaler=%d, Period=%d => 1ms 中断\r\n", 72-1, 1000-1);
    printf("LED: PB13, 每 1s 翻转一次\r\n");
    printf("开始定时...\r\n");

    while (1) {
        /* 主循环轮询 timer_ms，每 1000ms（1s）翻转 LED */
        if (timer_ms - last_toggle >= 1000) {
            last_toggle = timer_ms;

            /* 翻转 LED（GPIO ODR 异或） */
            if (GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_GPIO_PIN))
                GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
            else
                GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);

            printf("[%lu ms] LED toggled\r\n", timer_ms);
        }
    }
}
