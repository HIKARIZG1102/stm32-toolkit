MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
  RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .isr_vector : { KEEP(*(.isr_vector)) } > FLASH

  .text : {
    *(.text*)
    *(.glue_7*)
    *(.glue_7t*)
    *(.ARM.extab*)
    *(.ARM.exidx*)
  } > FLASH

  .rodata : { *(.rodata*) } > FLASH

  . = ALIGN(4);
  _sidata = .;

  .data : AT(_sidata)
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM

  _end = .;
  PROVIDE(end = _end);
}
