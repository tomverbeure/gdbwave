
#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;

#include "CpuTrace.h"
#include "Logger.h"

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

    if (strstr((char *)value, "x") || strstr((char *)value, "z")){
        return;
    }

    uint64_t valueInt = stol(string((const char *)value), nullptr, 2);
    LOG_DEBUG("%ld, %ud, %s, %s, %ld", time, signal->handle, signal->name.c_str(), value, valueInt);

    if (signal->handle == cpuTrace->pcValid.handle){
        cpuTrace->curPcValidVal   = valueInt;
        return;
    }

    if (signal->handle == cpuTrace->pc.handle){
        cpuTrace->curPcVal    = valueInt;
        return;
    }

    // All signals changes on the rising edge of the clock. Everything is stable at the falling edge...
    if (signal->handle == cpuTrace->clk.handle && valueInt == 0){
        if (cpuTrace->curPcValidVal){
            LOG_INFO("instr retire: %ld, %08lx", time, cpuTrace->curPcVal);

            PcValue     pc = { time, cpuTrace->curPcVal };
            cpuTrace->pcTrace.push_back(pc);
        }
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
        fstProc.reportSignalsNotFound(sigs);
        exit(-1);
    }

#if 0
    for(auto sig: sigs){
        LOG_DEBUG(sig->name + "," + std::to_string(sig->handle);
    }
#endif

    curPcValidVal   = 0;
    curPcVal        = 0;

    fstProc.getValueChanges(sigs, pcChangedCB, (void *)this);

    LOG_INFO("Nr CPU instructions: %ld", pcTrace.size());
}

