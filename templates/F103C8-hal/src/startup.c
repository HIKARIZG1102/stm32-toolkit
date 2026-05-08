/* startup.c — C 运行时启动
 *
 * 在进入 main() 前完成：
 *  - 清零 .bss 段
 *  - 从 Flash 拷贝 .data 段到 RAM
 *  - 调用 SystemInit() 初始化时钟
 *  - 调用 __libc_init_array() 初始化 C 库
 *
 * 注意：STM32F103 中断向量表的 16 个内部异常后紧跟
 * 设备特定中断。startup.S 的 .isr_vector 段定义了这些
 * 条目，本文件只是 C 运行时入口。
 */

#include <stdint.h>
#include "stm32f1xx.h"

/* 链接器符号 —— 由 link.ld 提供 */
extern uint32_t _sdata, _edata, _sbss, _ebss;
extern uint32_t _sidata;

void __attribute__((noreturn, weak)) _start(void)
{
    /* 拷贝 .data 段从 Flash 到 RAM */
    uint32_t *src   = &_sidata;
    uint32_t *dst   = &_sdata;
    while (dst < &_edata) *dst++ = *src++;

    /* 清零 .bss 段 */
    dst = &_sbss;
    while (dst < &_ebss) *dst++ = 0;

    /* 调用 CMSIS SystemInit（设置时钟、Flash 等待周期等） */
    SystemInit();

    /* C++ 全局构造（若使用） */
    extern void __libc_init_array(void);
    __libc_init_array();

    /* 进入用户 main */
    extern int main(void);
    main();

    /* main 返回后死循环 */
    while (1) __asm__("wfi");
}
