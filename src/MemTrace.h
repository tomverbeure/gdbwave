#ifndef MEM_TRACE_H
#define MEM_TRACE_H

#include <stdint.h>

#include <FstProcess.h>

struct MemAccess
{
    uint64_t    time;
    bool        wr;
    uint64_t    addr;
    uint64_t    value;
};

class MemTrace
{
public:
    MemTrace(FstProcess & fstProc, FstSignal clk, FstSignal memWr, FstSignal memAddr, FstSignal memWrData);
    void init();

    //void findNextMemAccess(uint64_t startTime, uint64_t pc_value);

    // Object to manage access to FST file
    FstProcess &    fstProc;

    // Handles to signals inside the FST file
    FstSignal       clk;
    FstSignal       memWr;
    FstSignal       memAddr;
    FstSignal       memWrData;

    // Helper signals for the FST callbacks to extract the PC values
    bool            curMemWr;
    uint64_t        curMemAddr;
    uint64_t        curMemWrData;

    // All PC values in the FST trace
    vector<MemAccess>   memTrace;

    vector<MemAccess>::iterator memTraceIt;

    bool getValue(uint64_t time, uint64_t addr, uint64_t *value);
};

#endif
