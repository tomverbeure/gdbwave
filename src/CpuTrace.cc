
#include <iostream>
#include <string>

using namespace std;

#include "CpuTrace.h"

CpuTrace::CpuTrace(FstProcess & fstProc, FstSignal clk, FstSignal pcValid, FstSignal pc) :
    fstProc(fstProc), 
    clk(clk),
    pcValid(pcValid),
    pc(pc)
{
    init();
}

static void pcChangedCB(uint64_t time, FstSignal *signal, const unsigned char *value, void *userInfo)
{
    CpuTrace *cpuTrace = (CpuTrace *)userInfo;

    uint64_t valueInt = stol(string((const char *)value), nullptr, 2);
//    cout << time << "," << signal->handle << "," << signal->name << "," << value << "," << valueInt << endl;

    if (signal->handle == cpuTrace->clk.handle && valueInt == 1){
        cout << time << "," << signal->name << "," << value << endl;
    }
}

void CpuTrace::init()
{
    vector<FstSignal *> sigs;

    sigs.push_back(&clk);
    sigs.push_back(&pcValid);
    sigs.push_back(&pc);

    bool allSigsFound = fstProc.assignHandles(sigs);
    if (!allSigsFound){
        cout << "Not all signals found..." << endl;
        exit(-1);
    }

#if 0
    for(auto sig: sigs){
        cout << sig->name << "," << sig->handle << endl;
    }
#endif

    fstProc.getValueChanges(sigs, pcChangedCB, (void *)this);
}

void CpuTrace::findNextPcValue(uint64_t startTime, uint64_t pc_value)
{
}
