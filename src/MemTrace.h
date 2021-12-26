#ifndef MEM_TRACE_H
#define MEM_TRACE_H

#include <stdint.h>
#include <vector>

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
    MemTrace(FstProcess & fstProc, FstSignal clk, 
                FstSignal memCmdValid, FstSignal memCmdReady, FstSignal memCmdAddr, FstSignal memCmdSize, FstSignal memCmdWr, FstSignal memCmdWrData,
                FstSignal memRspValid, FstSignal memRspData);

    void init();

    //void findNextMemAccess(uint64_t startTime, uint64_t pc_value);

    // Object to manage access to FST file
    FstProcess &    fstProc;

    // Handles to signals inside the FST file
    FstSignal       clk;
    FstSignal       memCmdValid; 
    FstSignal       memCmdReady; 
    FstSignal       memCmdAddr; 
    FstSignal       memCmdSize; 
    FstSignal       memCmdWr; 
    FstSignal       memCmdWrData;
    FstSignal       memRspValid; 
    FstSignal       memRspData;

    // Helper signals for the FST callbacks to extract the PC values
    bool            curMemCmdValid; 
    bool            curMemCmdReady; 
    uint64_t        curMemCmdAddr; 
    uint64_t        curMemCmdSize; 
    bool            curMemCmdWr; 
    uint64_t        curMemCmdWrData;
    bool            curMemRspValid; 
    uint64_t        curMemRspData;

    // All PC values in the FST trace
    vector<MemAccess>   memTrace;

    vector<MemAccess>::iterator memTraceIt;

    void processSignalChanged(uint64_t time, FstSignal *signal, const unsigned char *value);

    bool getValue(uint64_t time, uint64_t addr, uint64_t *value);
};

#endif
