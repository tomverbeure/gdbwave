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

    // Object to manage access to FST file
    FstProcess &    fstProc;

    // Handles to signals inside the FST file
    FstSignal       clk;
    FstSignal       pcValid;
    FstSignal       pc;

    // Helper signals for the FST callbacks to extract the PC values
    uint64_t        curPcValidVal;
    uint64_t        curPcVal;

    // All PC values in the FST trace
    vector<PcValue>     pcTrace;

    vector<PcValue>::iterator pcTraceIt;
};

#endif
