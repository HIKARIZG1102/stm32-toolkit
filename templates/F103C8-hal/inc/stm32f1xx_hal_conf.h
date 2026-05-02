/**
  ******************************************************************************
  * @file    stm32f1xx_hal_conf.h
  * @brief   HAL configuration file.
  ******************************************************************************
  */
#ifndef __STM32F1xx_HAL_CONF_H
#define __STM32F1xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* 直接引用芯片级头文件，不走 stm32f1xx.h（避免循环 include） */
#include "stm32f103xb.h"
#include "stm32f1xx_hal_def.h"

#define HSE_VALUE           ((uint32_t)8000000)
#define HSI_VALUE           ((uint32_t)8000000)
#define LSE_VALUE           ((uint32_t)32768)
#define LSI_VALUE           ((uint32_t)40000)

#define HAL_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED

#define TICK_INT_PRIORITY   ((uint32_t)0x0F)
#define assert_param(expr)  ((void)0)

#if defined(HAL_CORTEX_MODULE_ENABLED)
#include "stm32f1xx_hal_cortex.h"
#endif
#if defined(HAL_GPIO_MODULE_ENABLED)
#include "stm32f1xx_hal_gpio.h"
#endif
#if defined(HAL_RCC_MODULE_ENABLED)
#include "stm32f1xx_hal_rcc.h"
#endif
#if defined(HAL_FLASH_MODULE_ENABLED)
#include "stm32f1xx_hal_flash.h"
#endif
#if defined(HAL_DMA_MODULE_ENABLED)
#include "stm32f1xx_hal_dma.h"
#endif
#if defined(HAL_UART_MODULE_ENABLED)
#include "stm32f1xx_hal_uart.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_HAL_CONF_H */
