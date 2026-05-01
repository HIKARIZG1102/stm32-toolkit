#include <stdint.h>
#define SP 0x20005000
void main(void);
__attribute__((used,section(".isr_vector")))
void(*const v[])(void)={(void(*)())SP,main};
void main(void){while(1){}}
