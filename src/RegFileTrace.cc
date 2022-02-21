
#include <stdio.h>

#include <iostream>
#include <string>

using namespace std;

#include "RegFileTrace.h"
#include "Logger.h"

extern bool verbose;

RegFileTrace::RegFileTrace(FstProcess & fstProc, FstSignal clk, FstSignal memWr, FstSignal memAddr, FstSignal memWrData) :
    fstProc(fstProc), 
    clk(clk),
    memWr(memWr),
    memAddr(memAddr),
    memWrData(memWrData)
{
    init();
}

static void memChangedCB(uint64_t time, FstSignal *signal, const unsigned char *value, void *userInfo)
{
    RegFileTrace *regFileTrace = (RegFileTrace *)userInfo;

    if (strstr((char *)value, "x") || strstr((char *)value, "z")){
        return;
    }

    uint64_t valueInt = stol(string((const char *)value), nullptr, 2);

#if 0
    LOG_DEBUG("%ld, %ud, %s, %s, %ld", time, signal->handle, signal->name.c_str(), value, valueInt);
#endif

    if (signal->handle == regFileTrace->memWr.handle){
        regFileTrace->curMemWr      = valueInt;
        return;
    }

    if (signal->handle == regFileTrace->memAddr.handle){
        regFileTrace->curMemAddr    = valueInt;
        return;
    }

    if (signal->handle == regFileTrace->memWrData.handle){
        regFileTrace->curMemWrData  = valueInt;
        return;
    }

    // All signals changes on the rising edge of the clock. Everything is stable at the falling edge...
    if (signal->handle == regFileTrace->clk.handle && valueInt == 0){
        if (regFileTrace->curMemWr){
            if (verbose) LOG_INFO("RegWr: 0x%08lx <- 0x%08lx (@%ld)", regFileTrace->curMemAddr, regFileTrace->curMemWrData, time);

            RegFileAccess   mem = { time, regFileTrace->curMemWr, regFileTrace->curMemAddr, regFileTrace->curMemWrData };
            regFileTrace->regFileTrace.push_back(mem);
        }
    }
}

void RegFileTrace::init()
{
    vector<FstSignal *> sigs;

    sigs.push_back(&clk);
    sigs.push_back(&memWr);
    sigs.push_back(&memAddr);
    sigs.push_back(&memWrData);

    bool allSigsFound = fstProc.assignHandles(sigs);
    if (!allSigsFound){
        fstProc.reportSignalsNotFound(sigs);
        exit(-1);
    }

    curMemWr        = false;
    curMemAddr      = 0;
    curMemWrData    = 0;

    fstProc.getValueChanges(sigs, memChangedCB, (void *)this);

    LOG_INFO("Nr regfile write transactions: %ld", regFileTrace.size());
}


bool RegFileTrace::getValue(uint64_t time, uint64_t addr, uint64_t *value)
{
    bool        valueValid = false;
    uint64_t    val;

    for(auto m: regFileTrace){
        if (m.time > time)
            break;

        if (m.wr && m.addr == addr){
            valueValid      = true;
            val             = m.value;
        }
    }

    if (valueValid){
        *value      = val;
    }

    return valueValid;
}


