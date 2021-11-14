#ifndef CPU_TRACE_H
#define CPU_TRACE_H

#include <stdint.h>

#include <FstProcess.h>

class CpuTrace
{
public:
    CpuTrace(FstProcess & fstProc, FstSignal clk, FstSignal pcValid, FstSignal pc);

    void findNextPcValue(uint64_t startTime, uint64_t pc_value);

    FstProcess &    fstProc;
    FstSignal       clk;
    FstSignal       pcValid;
    FstSignal       pc;
};

#endif
