/* main.c -- ADC_Light: Read photosensitive sensor via ADC1 + interrupt
 * ADC1 channel 0 (PA0), conversion complete interrupt
 * Chip: STM32F103RCT6 (HD), SPL library
 *
 * Wiring:
 *   PA0 (ADC12_IN0) <- AO (light sensor module analog out)
 *   VCC <- 3.3V
 *   GND <- GND
 */

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include <stdio.h>

/* ====== Global Variables ====== */
volatile uint16_t adc_value = 0;     /* Latest ADC value (updated in ISR) */
volatile uint8_t  adc_ready = 0;     /* New data ready flag */

/* ====== USART1 Init ====== */
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

/* ====== ADC1 + Interrupt Init ====== */
static void ADC1_Init_IT(void)
{
    GPIO_InitTypeDef gpio;
    ADC_InitTypeDef adc;
    NVIC_InitTypeDef nvic;

    /* GPIO clock: PA0 = analog input */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = GPIO_Pin_0;
    gpio.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &gpio);

    /* ADC1 config */
    ADC_StructInit(&adc);
    adc.ADC_Mode               = ADC_Mode_Independent;
    adc.ADC_ScanConvMode       = DISABLE;
    adc.ADC_ContinuousConvMode = ENABLE;
    adc.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    adc.ADC_DataAlign          = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &adc);

    /* ADC clock: PCLK2/8 = 9MHz */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    /* Channel 0 (PA0), sample time 55.5 cycles */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

    /* Enable end-of-conversion interrupt */
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

    /* NVIC config */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic.NVIC_IRQChannel                   = ADC1_2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority        = 0;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* Calibration */
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    /* Start continuous conversion */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/* ====== ADC1_2 Interrupt Handler ====== */
void ADC1_2_IRQHandler(void)
{
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC)) {
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
        adc_value = ADC_GetConversionValue(ADC1);
        adc_ready = 1;
    }
}

/* ====== Simple Delay ====== */
static void Delay(__IO uint32_t n)
{
    for (; n != 0; n--);
}

/* ====== Main ====== */
int main(void)
{
    uint16_t last_val = 0xFFFF;

    USART1_Config();
    ADC1_Init_IT();

    printf("\r\n========== ADC_Light (ADC1 + Interrupt) ==========\r\n");
    printf("Light sensor -> PA0 (ADC1_IN0)\r\n");
    printf("Continuous conversion + ISR read\r\n");
    printf("ADC range: 0 (brightest) ~ 4095 (darkest)\r\n");
    printf("================================================\r\n");

    while (1)
    {
        if (adc_ready) {
            adc_ready = 0;

            /* Only print when value changes by >10 (debounce) */
            if (adc_value > last_val + 10 || adc_value < last_val - 10 || last_val == 0xFFFF) {
                last_val = adc_value;
                printf("ADC = %4d  (0x%04X)  |  Light level: %d/4095\r\n",
                       adc_value, adc_value, adc_value);
            }
            Delay(50000);
        }
    }
}
