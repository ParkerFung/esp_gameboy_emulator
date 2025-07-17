#ifndef CPU_H
#define CPU_H

#include "rom.h"
#include <stdint.h>  // Needed for uint64_t, uint32_t

void cpu_init(void);
int cpu_cycle(void);
unsigned int cpu_get_cycles(void);
void cpu_interrupt(unsigned short);


#endif
