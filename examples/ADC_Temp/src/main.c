/* main.c — ADC_Temp：读取 STM32 芯片内部温度传感器 (ADC1_CH16)
 * 复现自：8-3-ADC-芯片温度（ADC1_ch16）
 * 使用 DMA 循环传输，主循环读取 ADC 值并计算温度
 * 芯片：STM32F103RCT6（HD），SPL 库
 */

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include <stdio.h>

/* ═══════════════════════ 宏定义 ═══════════════════════ */
/* ADC1 数据寄存器地址 */
#define ADC1_DR_Address    ((uint32_t)ADC1_BASE + 0x4C)

/* 温度计算参数（12-bit ADC, VREF=3.3V）
 * V25 = 1.43V → ADC 值 = 1.43 * 4095 / 3.3 ≈ 1774 = 0x6EE
 * AVG_SLOPE = 4.3 mV/°C → ADC 值/°C = 0.0043 * 4095 / 3.3 ≈ 5.33 ≈ 0x05
 */
#define V25         0x6EE
#define AVG_SLOPE   0x05

/* ═══════════════════════ 全局变量 ═══════════════════════ */
volatile uint16_t ADC_ConvertedValue;   /* DMA 自动更新的 ADC 值 */
uint16_t Current_Temp;                  /* 计算出的温度值 */

/* ═══════════════════════ 简单延时 ═══════════════════════ */
static void Delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}

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

/* 重定向 printf 到 USART1 */
int _write(int file, char *ptr, int len)
{
    (void)file;
    for (int i = 0; i < len; i++) {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = (uint8_t)ptr[i];
    }
    return len;
}

/* ═══════════════════════ ADC1 + DMA 初始化 ═══════════════════════ */
static void Temp_ADC1_Init(void)
{
    DMA_InitTypeDef dma;
    ADC_InitTypeDef adc;

    /* ── 时钟使能 ── */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* ── DMA1 Channel1 配置：ADC1_DR → ADC_ConvertedValue ── */
    DMA_DeInit(DMA1_Channel1);
    DMA_StructInit(&dma);
    dma.DMA_PeripheralBaseAddr = ADC1_DR_Address;          /* 外设地址：ADC 数据寄存器 */
    dma.DMA_MemoryBaseAddr     = (uint32_t)&ADC_ConvertedValue; /* 内存地址 */
    dma.DMA_DIR                = DMA_DIR_PeripheralSRC;     /* 外设 → 内存 */
    dma.DMA_BufferSize         = 1;                         /* 传输 1 个半字 */
    dma.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc          = DMA_MemoryInc_Disable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; /* 16 位 */
    dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dma.DMA_Mode               = DMA_Mode_Circular;         /* 循环模式：持续更新 */
    dma.DMA_Priority           = DMA_Priority_High;
    dma.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &dma);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ── ADC1 配置 ── */
    ADC_StructInit(&adc);
    adc.ADC_Mode               = ADC_Mode_Independent;      /* 独立模式 */
    adc.ADC_ScanConvMode       = ENABLE;                    /* 扫描模式（单通道也需开启） */
    adc.ADC_ContinuousConvMode = ENABLE;                    /* 连续转换 */
    adc.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None; /* 软件触发 */
    adc.ADC_DataAlign          = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel       = 1;                         /* 1 个通道 */
    ADC_Init(ADC1, &adc);

    /* ADC 时钟：PCLK2/8 = 72MHz/8 = 9MHz（≤14MHz 规范要求） */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    /* 配置通道 16（内部温度传感器），采样时间 239.5 周期 */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);

    /* 使能温度传感器和内部参考电压 */
    ADC_TempSensorVrefintCmd(ENABLE);

    /* 使能 ADC1 的 DMA 请求 */
    ADC_DMACmd(ADC1, ENABLE);

    /* 使能 ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* 校准 */
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    /* 开始连续转换 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/* ═══════════════════════ 主函数 ═══════════════════════ */
int main(void)
{
    USART1_Config();
    Temp_ADC1_Init();

    printf("\r\n========== ADC_Temp (ADC1 内部温度传感器) ==========\r\n");
    printf("ADC1_CH16: 内部温度传感器\r\n");
    printf("DMA1_CH1: 循环传输 ADC 值\r\n");
    printf("V25 = 0x%04X, AVG_SLOPE = 0x%02X\r\n", V25, AVG_SLOPE);
    printf("====================================================\r\n");

    while (1)
    {
        Delay(0xFFFFFF);  /* 延时约 0.5s */

        /* 温度计算公式 (参考 STM32 参考手册):
         * Current_Temp = (V25 - VSENSE) / Avg_Slope + 25
         * 使用 ADC 计数值近似计算 */
        Current_Temp = (V25 - ADC_ConvertedValue) / AVG_SLOPE + 25;

        printf("ADC_raw = %4d  (0x%04X)  |  Temp = %2d °C\r\n",
               ADC_ConvertedValue, ADC_ConvertedValue, Current_Temp);
    }
}
