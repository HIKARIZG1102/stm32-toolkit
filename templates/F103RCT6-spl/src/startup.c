/* startup.c -- STM32F103 startup (full vector table)
 *
 * Features:
 *   1. Full interrupt vector table (all entries -> Default_Handler)
 *   2. .data copy from flash to RAM
 *   3. .bss zero-initialization
 *   4. SystemInit() -> main()
 *
 * Usage:
 *   To use an interrupt, define the IRQHandler anywhere in your project:
 *     void TIM2_IRQHandler(void) {
 *         // your code here
 *     }
 *   The strong symbol overrides the weak default in this file.
 */

#include <stdint.h>

/* ── 外部符号（来自链接脚本和用户代码） ── */
extern void main(void);
extern void SystemInit(void);
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

void Reset_Handler(void);

/* ══════════════════════════════════════════════════════════════
 *  默认中断处理（死循环）
 *  用户只需定义所需的 IRQHandler，自动覆盖弱符号
 * ══════════════════════════════════════════════════════════════ */
void __attribute__((weak, noreturn)) Default_Handler(void) { while (1); }

/* ── 内核异常 ── */
void NMI_Handler(void)              __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)         __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)              __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)         __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)           __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)          __attribute__((weak, alias("Default_Handler")));

/* ── 外设中断（HD: 82 项向量表） ── */
void WWDG_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void TAMPER_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void RTC_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void RTCAlarm_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void USBWakeUp_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void TIM8_BRK_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void TIM8_UP_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void TIM8_TRG_COM_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void TIM8_CC_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void ADC3_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void FSMC_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void SDIO_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void TIM5_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void SPI3_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void UART4_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void UART5_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void TIM6_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void TIM7_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel1_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel2_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel3_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel4_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel5_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void ETH_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void ETH_WKUP_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void CAN2_TX_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void CAN2_RX0_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void CAN2_RX1_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void CAN2_SCE_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));

/* ══════════════════════════════════════════════════════════════
 *  中断向量表（82 项，STM32F103 HD）
 *  首项 = 栈顶，次项 = 复位入口，其余 = 默认中断处理
 * ══════════════════════════════════════════════════════════════ */
void __attribute__((section(".isr_vector"), used)) (*const v[])(void) = {
    (void(*)())&_estack,           /* 0: MSP 栈顶 */
    Reset_Handler,                 /* 1: 复位 */
    NMI_Handler,                   /* 2: NMI */
    HardFault_Handler,             /* 3: HardFault */
    MemManage_Handler,             /* 4: MemManage */
    BusFault_Handler,              /* 5: BusFault */
    UsageFault_Handler,            /* 6: UsageFault */
    0,0,0,0,                       /* 7-10: Reserved */
    SVC_Handler,                   /* 11: SVCall */
    DebugMon_Handler,              /* 12: DebugMon */
    0,                             /* 13: Reserved */
    PendSV_Handler,                /* 14: PendSV */
    SysTick_Handler,               /* 15: SysTick */
    WWDG_IRQHandler,               /* 16 */
    PVD_IRQHandler,                /* 17 */
    TAMPER_IRQHandler,             /* 18 */
    RTC_IRQHandler,                /* 19 */
    FLASH_IRQHandler,              /* 20 */
    RCC_IRQHandler,                /* 21 */
    EXTI0_IRQHandler,              /* 22 */
    EXTI1_IRQHandler,              /* 23 */
    EXTI2_IRQHandler,              /* 24 */
    EXTI3_IRQHandler,              /* 25 */
    EXTI4_IRQHandler,              /* 26 */
    DMA1_Channel1_IRQHandler,      /* 27 */
    DMA1_Channel2_IRQHandler,      /* 28 */
    DMA1_Channel3_IRQHandler,      /* 29 */
    DMA1_Channel4_IRQHandler,      /* 30 */
    DMA1_Channel5_IRQHandler,      /* 31 */
    DMA1_Channel6_IRQHandler,      /* 32 */
    DMA1_Channel7_IRQHandler,      /* 33 */
    ADC1_2_IRQHandler,             /* 34 */
    USB_HP_CAN1_TX_IRQHandler,     /* 35 */
    USB_LP_CAN1_RX0_IRQHandler,    /* 36 */
    CAN1_RX1_IRQHandler,           /* 37 */
    CAN1_SCE_IRQHandler,           /* 38 */
    EXTI9_5_IRQHandler,            /* 39 */
    TIM1_BRK_IRQHandler,           /* 40 */
    TIM1_UP_IRQHandler,            /* 41 */
    TIM1_TRG_COM_IRQHandler,       /* 42 */
    TIM1_CC_IRQHandler,            /* 43 */
    TIM2_IRQHandler,               /* 44 */
    TIM3_IRQHandler,               /* 45 */
    TIM4_IRQHandler,               /* 46 */
    I2C1_EV_IRQHandler,            /* 47 */
    I2C1_ER_IRQHandler,            /* 48 */
    I2C2_EV_IRQHandler,            /* 49 */
    I2C2_ER_IRQHandler,            /* 50 */
    SPI1_IRQHandler,               /* 51 */
    SPI2_IRQHandler,               /* 52 */
    USART1_IRQHandler,             /* 53 */
    USART2_IRQHandler,             /* 54 */
    USART3_IRQHandler,             /* 55 */
    EXTI15_10_IRQHandler,          /* 56 */
    RTCAlarm_IRQHandler,           /* 57 */
    USBWakeUp_IRQHandler,          /* 58 */
    TIM8_BRK_IRQHandler,           /* 59 */
    TIM8_UP_IRQHandler,            /* 60 */
    TIM8_TRG_COM_IRQHandler,       /* 61 */
    TIM8_CC_IRQHandler,            /* 62 */
    ADC3_IRQHandler,               /* 63 */
    FSMC_IRQHandler,               /* 64 */
    SDIO_IRQHandler,               /* 65 */
    TIM5_IRQHandler,               /* 66 */
    SPI3_IRQHandler,               /* 67 */
    UART4_IRQHandler,              /* 68 */
    UART5_IRQHandler,              /* 69 */
    TIM6_IRQHandler,               /* 70 */
    TIM7_IRQHandler,               /* 71 */
    DMA2_Channel1_IRQHandler,      /* 72 */
    DMA2_Channel2_IRQHandler,      /* 73 */
    DMA2_Channel3_IRQHandler,      /* 74 */
    DMA2_Channel4_IRQHandler,      /* 75 */
    DMA2_Channel5_IRQHandler,      /* 76 */
    ETH_IRQHandler,                /* 77 */
    ETH_WKUP_IRQHandler,           /* 78 */
    CAN2_TX_IRQHandler,            /* 79 */
    CAN2_RX0_IRQHandler,           /* 80 */
    CAN2_RX1_IRQHandler,           /* 81 */
    CAN2_SCE_IRQHandler,           /* 82 */
    OTG_FS_IRQHandler,             /* 83 */
};

/* ══════════════════════════════════════════════════════════════
 *  复位入口
 *  顺序:
 *   1. 复制 .data 段（Flash → RAM）
 *   2. 清零 .bss  段
 *   3. 系统时钟初始化
 *   4. 跳转 main()
 * ══════════════════════════════════════════════════════════════ */
void __attribute__((naked)) Reset_Handler(void)
{
    uint32_t *src, *dst;

    /* 复制 .data */
    for (src = &_sidata, dst = &_sdata; dst < &_edata; src++, dst++)
        *dst = *src;

    /* 清零 .bss */
    for (dst = &_sbss; dst < &_ebss; dst++)
        *dst = 0;

    SystemInit();
    main();

    /* main() 不应返回，但防御性处理 */
    while (1);
}
