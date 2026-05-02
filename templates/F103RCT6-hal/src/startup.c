/* startup.c — STM32F103 启动文件（完整向量表版） */
extern void main(void);
extern void SystemInit(void);

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

extern uint32_t _estack;

/* 默认异常处理（死循环） */
void __attribute__((weak, noreturn)) Default_Handler(void) {
    while (1);
}
void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)    __attribute__((weak, alias("Default_Handler")));

/* 完整向量表 */
void __attribute__((section(".isr_vector"), used)) (*const v[])(void) = {
    (void(*)())&_estack,        /* 0: SP */
    Reset_Handler,              /* 1: Reset */
    NMI_Handler,                /* 2: NMI */
    HardFault_Handler,          /* 3: HardFault */
    MemManage_Handler,          /* 4: MemManage */
    BusFault_Handler,           /* 5: BusFault */
    UsageFault_Handler,         /* 6: UsageFault */
    0,0,0,0,                    /* 7-10: Reserved */
    SVC_Handler,                /* 11: SVCall */
    DebugMon_Handler,           /* 12: DebugMon */
    0,                          /* 13: Reserved */
    PendSV_Handler,             /* 14: PendSV */
    SysTick_Handler,            /* 15: SysTick */
    /* 以下外设中断未使用，全部指向 Default_Handler（可通过弱符号覆盖） */
    [16 ... 67] = Default_Handler,
};

void __attribute__((naked)) Reset_Handler(void) {
    /* 复制 .data 从 Flash 到 RAM */
    for (uint32_t *src = &_sidata, *dst = &_sdata; dst < &_edata; src++, dst++)
        *dst = *src;

    /* 清零 .bss */
    for (uint32_t *dst = &_sbss; dst < &_ebss; dst++)
        *dst = 0;

    SystemInit();
    main();
    while (1);
}
