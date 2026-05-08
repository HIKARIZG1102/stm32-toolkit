/* startup.c — STM32F103 启动文件（完整向量表，适配 SPL + gcc -nostartfiles） */
#include <stdint.h>

extern void main(void);
extern void SystemInit(void);

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

/* 前向声明 */
void Reset_Handler(void);

/* ── 默认异常处理（死循环） ── */
void __attribute__((weak, noreturn)) Default_Handler(void) { while (1); }

void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)    __attribute__((weak, alias("Default_Handler")));

/* ── TIM 中断处理（在 main.c 中强符号定义） ── */
void TIM2_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));

/* 外部中断 */
void WWDG_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void TAMPER_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void RTC_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void RTCAlarm_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void USBWakeUp_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));

/* ── 完整向量表（STM32F103 HD: 82 个中断） ── */
void __attribute__((section(".isr_vector"), used)) (*const v[])(void) = {
    (void(*)())&_estack,       /* 0: SP */
    Reset_Handler,             /* 1: Reset */
    NMI_Handler,               /* 2: NMI */
    HardFault_Handler,         /* 3: HardFault */
    MemManage_Handler,         /* 4: MemManage */
    BusFault_Handler,          /* 5: BusFault */
    UsageFault_Handler,        /* 6: UsageFault */
    0,0,0,0,                   /* 7-10: Reserved */
    SVC_Handler,               /* 11: SVCall */
    DebugMon_Handler,          /* 12: DebugMon */
    0,                         /* 13: Reserved */
    PendSV_Handler,            /* 14: PendSV */
    SysTick_Handler,           /* 15: SysTick */
    WWDG_IRQHandler,           /* 16: WWDG */
    PVD_IRQHandler,            /* 17: PVD */
    TAMPER_IRQHandler,         /* 18: TAMPER */
    RTC_IRQHandler,            /* 19: RTC */
    FLASH_IRQHandler,          /* 20: FLASH */
    RCC_IRQHandler,            /* 21: RCC */
    EXTI0_IRQHandler,          /* 22: EXTI0 */
    EXTI1_IRQHandler,          /* 23: EXTI1 */
    EXTI2_IRQHandler,          /* 24: EXTI2 */
    EXTI3_IRQHandler,          /* 25: EXTI3 */
    EXTI4_IRQHandler,          /* 26: EXTI4 */
    DMA1_Channel1_IRQHandler,  /* 27 */
    DMA1_Channel2_IRQHandler,  /* 28 */
    DMA1_Channel3_IRQHandler,  /* 29 */
    DMA1_Channel4_IRQHandler,  /* 30 */
    DMA1_Channel5_IRQHandler,  /* 31 */
    DMA1_Channel6_IRQHandler,  /* 32 */
    DMA1_Channel7_IRQHandler,  /* 33 */
    ADC1_2_IRQHandler,         /* 34 */
    USB_HP_CAN1_TX_IRQHandler, /* 35 */
    USB_LP_CAN1_RX0_IRQHandler,/* 36 */
    CAN1_RX1_IRQHandler,       /* 37 */
    CAN1_SCE_IRQHandler,       /* 38 */
    EXTI9_5_IRQHandler,        /* 39 */
    TIM1_BRK_IRQHandler,       /* 40 */
    TIM1_UP_IRQHandler,        /* 41 */
    TIM1_TRG_COM_IRQHandler,   /* 42 */
    TIM1_CC_IRQHandler,        /* 43 */
    TIM2_IRQHandler,           /* 44 */
    0, 0, 0,                   /* 45-47: TIM3, TIM4, I2C1_EV */
    I2C1_ER_IRQHandler,        /* 48 */
    0, 0,                      /* 49-50: I2C2_EV, I2C2_ER */
    SPI1_IRQHandler,           /* 51 */
    SPI2_IRQHandler,           /* 52 */
    USART1_IRQHandler,         /* 53 */
    USART2_IRQHandler,         /* 54 */
    USART3_IRQHandler,         /* 55 */
    EXTI15_10_IRQHandler,      /* 56 */
    RTCAlarm_IRQHandler,       /* 57 */
    USBWakeUp_IRQHandler,      /* 58 */
    [59 ... 81] = Default_Handler,
};

void __attribute__((naked)) Reset_Handler(void) {
    uint32_t *src, *dst;
    /* 复制 .data 从 Flash 到 RAM */
    for (src = &_sidata, dst = &_sdata; dst < &_edata; src++, dst++)
        *dst = *src;
    /* 清零 .bss */
    for (dst = &_sbss; dst < &_ebss; dst++)
        *dst = 0;
    SystemInit();
    main();
    while (1);
}
