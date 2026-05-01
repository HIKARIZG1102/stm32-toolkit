MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
  RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}
SECTIONS
{
  .isr_vector : { KEEP(*(.isr_vector)) } > FLASH
  .text : { *(.text*) } > FLASH
  .rodata : { *(.rodata*) } > FLASH
}
_SYSTEM_MEMORY_START = 0x1FFFF000;
_SYSTEM_MEMORY_END   = 0x1FFFF800;
