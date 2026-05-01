#include <stdint.h>
#define SP 0x2000C000
int main(void);
__attribute__((used,section(".isr_vector")))
void(*const v[])(void)={(void(*)())SP,main};
int main(void){while(1){}}
