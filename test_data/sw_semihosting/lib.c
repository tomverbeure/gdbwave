
#include <stdint.h>

#include "reg.h"
#include "top_defines.h"
#include "lib.h"

static inline uint32_t internal_rdcycle(void) {
    uint32_t cycle;
    asm volatile ("rdcycle %0" : "=r"(cycle));
    return cycle;
}

static inline uint32_t internal_rdcycleh(void) {
    uint32_t cycle;
    asm volatile ("rdcycleh %0" : "=r"(cycle));
    return cycle;
}

uint64_t rdcycle64(void) {

    uint32_t msw;
    uint32_t lsw;

    do{
        msw = internal_rdcycleh();
        lsw = internal_rdcycle();
    } while(msw != internal_rdcycleh());

    return ((uint64_t)msw << 32) | lsw;
}

void wait_cycles(uint32_t cycles)
{
    uint64_t start;

    start = rdcycle64();
    while ((rdcycle64() - start) <= (uint64_t)cycles) {}
}


void wait_ms(uint32_t ms)
{
    wait_cycles((uint32_t)CPU_FREQ / 1000UL * ms);
}

void wait_us(uint32_t us)
{
    wait_cycles((uint32_t)CPU_FREQ / 1000000UL * us);
}
