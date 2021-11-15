#ifndef CPU_TRACE_H
#define CPU_TRACE_H

#include <stdint.h>

#include <FstProcess.h>

struct PcValue
{
    uint64_t    time;
    uint64_t    pc;
};

class CpuTrace
{
public:
    CpuTrace(FstProcess & fstProc, FstSignal clk, FstSignal pcValid, FstSignal pc);
    void init();

    void findNextPcValue(uint64_t startTime, uint64_t pc_value);

    FstProcess &    fstProc;
    FstSignal       clk;
    FstSignal       pcValid;
    FstSignal       pc;

    uint64_t        curPcValidVal;
    uint64_t        curPcVal;

    vector<PcValue>     pcTrace;
};

#endif
