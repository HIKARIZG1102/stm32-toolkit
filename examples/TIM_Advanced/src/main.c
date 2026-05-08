/* main.c — TIM_Advanced：高级定时器 TIM1 定时（1ms 中断，双路独立定时）
 * 复现自：7-2-TIM1-高级定时器-定时
 * 设计模式：1ms ISR 递增两个计时变量，主循环轮询实现双路不同周期
 * 芯片：STM32F103RCT6（HD），SPL 库
 * 
 * LED 行为说明（仅 PB13 一个 LED）：
 *   定时器 A（0.5s）：将 PB13 置为低电平（亮）
 *   定时器 B（1.0s）：将 PB13 置为高电平（灭）
 *   综合效果：LED 亮 0.5s → 灭 0.5s → 亮 0.5s → ...（1Hz，50% 占空比）
 *   但背后的两个独立定时器同时在工作！
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
volatile uint32_t timer_a = 0;   /* 用于 0.5s 定时（ISR 中递增） */
volatile uint32_t timer_b = 0;   /* 用于 1.0s 定时（ISR 中递增） */

/* ═══════════════════════ USART1 初始化 ═══════════════════════ */
static void USART1_Config(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin   = GPIO_Pin_10;
    gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    USART_StructInit(&usart);
    usart.USART_BaudRate = 115200;
    usart.USART_Mode     = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}

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

/* ═══════════════════════ TIM1 初始化（高级定时器） ═══════════════════════ */
static void TIM1_Config(void)
{
    TIM_TimeBaseInitTypeDef tim;
    NVIC_InitTypeDef nvic;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&tim);
    tim.TIM_Prescaler         = 71;
    tim.TIM_Period            = 1000 - 1;
    tim.TIM_ClockDivision     = TIM_CKD_DIV1;
    tim.TIM_CounterMode       = TIM_CounterMode_Up;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &tim);

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM1, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic.NVIC_IRQChannel                   = TIM1_UP_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority        = 3;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);
}

/* ═══════════════════════ TIM1 更新中断处理 ═══════════════════════ */
void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        timer_a++;    /* 两路计时独立递增 */
        timer_b++;
    }
}

/* ═══════════════════════ 主函数 ═══════════════════════ */
int main(void)
{
    uint32_t last_a = 0, last_b = 0;

    LED_Config();
    USART1_Config();
    TIM1_Config();

    printf("\r\n========== TIM_Advanced (TIM1 高级定时器) ==========\r\n");
    printf("TIM1 Prescaler=%d, Period=%d => 1ms 中断\r\n", 71, 1000-1);
    printf("LED (PB13): 两个独立定时器共同控制同一颗 LED\r\n");
    printf("  定时器A (0.5s): 点亮 LED\r\n");
    printf("  定时器B (1.0s): 熄灭 LED\r\n");
    printf("  综合效果: LED 亮 0.5s → 灭 0.5s → 亮 0.5s → ...\r\n");
    printf("开始定时...\r\n");

    while (1) {
        /* 定时器A：每 500ms 点亮 LED */
        if (timer_a - last_a >= 500) {
            last_a = timer_a;
            GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);  /* 亮（低电平）*/
            printf("[%lu ms] TimerA(0.5s): LED ON\r\n", timer_a);
        }

        /* 定时器B：每 1000ms 熄灭 LED */
        if (timer_b - last_b >= 1000) {
            last_b = timer_b;
            GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);    /* 灭（高电平）*/
            printf("[%lu ms] TimerB(1.0s): LED OFF\r\n", timer_b);
        }
    }
}
