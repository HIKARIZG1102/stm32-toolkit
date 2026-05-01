#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define SP 0x2000C000

void main(void);
__attribute__((used,section(".isr_vector")))
void(*const v[])(void)={(void(*)())SP,main};

void delay(volatile uint32_t c) { while(c--); }

void main(void) {
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_13;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &gpio);
    while(1){
        GPIO_ResetBits(GPIOB, GPIO_Pin_13);
        delay(500000);
        GPIO_SetBits(GPIOB, GPIO_Pin_13);
        delay(500000);
    }
}
