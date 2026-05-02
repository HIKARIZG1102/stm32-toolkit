/**
  ******************************************************************************
  * @file    system_stm32f10x.c
  * @brief   CMSIS Cortex-M3 System Source - STM32F10x
  *          默认 HSI 8MHz, 取消注释 PLL 部分切换到 72MHz
  ******************************************************************************
  */
#include "stm32f10x.h"

#define HSI_VALUE    ((uint32_t)8000000)
uint32_t SystemCoreClock = HSI_VALUE;

void SystemInit(void) {
    RCC->CR |= (uint32_t)0x00000001;
    RCC->CFGR &= (uint32_t)0xF8FF0000;
    RCC->CR &= ~(uint32_t)0x01000000;
    RCC->CIR = 0;
}

void SystemCoreClockUpdate(void) {
    SystemCoreClock = HSI_VALUE;
}
