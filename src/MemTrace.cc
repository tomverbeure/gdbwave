
#include <stdio.h>

#include <iostream>
#include <string>

using namespace std;

#include "MemTrace.h"

MemTrace::MemTrace(FstProcess & fstProc, FstSignal clk, FstSignal memWr, FstSignal memAddr, FstSignal memWrData) :
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
    MemTrace *memTrace = (MemTrace *)userInfo;

    uint64_t valueInt = stol(string((const char *)value), nullptr, 2);
    //cout << time << "," << signal->handle << "," << signal->name << "," << value << "," << valueInt << endl;

    if (signal->handle == memTrace->memWr.handle){
        memTrace->curMemWr      = valueInt;
        return;
    }

    if (signal->handle == memTrace->memAddr.handle){
        memTrace->curMemAddr    = valueInt;
        return;
    }

    if (signal->handle == memTrace->memWrData.handle){
        memTrace->curMemWrData  = valueInt;
        return;
    }

    // All signals changes on the rising edge of the clock. Everything is stable at the falling edge...
    if (signal->handle == memTrace->clk.handle && valueInt == 0){
        if (memTrace->curMemWr){
            printf("MemWr: 0x%08lx <- 0x%08lx (@%ld)\n", memTrace->curMemAddr, memTrace->curMemWrData, time);

            MemAccess   mem = { time, memTrace->curMemWr, memTrace->curMemAddr, memTrace->curMemWrData };
            memTrace->memTrace.push_back(mem);
        }
    }
}

void MemTrace::init()
{
    vector<FstSignal *> sigs;

    sigs.push_back(&clk);
    sigs.push_back(&memWr);
    sigs.push_back(&memAddr);
    sigs.push_back(&memWrData);

    bool allSigsFound = fstProc.assignHandles(sigs);
    if (!allSigsFound){
        cout << "Not all signals found..." << endl;

        for(auto sig: sigs){
            if (!sig->hasHandle){
                printf("Signal not found: %s. %s\n", sig->scopeName.c_str(), sig->name.c_str());
            }
        }

        exit(-1);
    }

    curMemWr        = false;
    curMemAddr      = 0;
    curMemWrData    = 0;

    fstProc.getValueChanges(sigs, memChangedCB, (void *)this);

    printf("Nr mem transactions: %ld\n", memTrace.size());
}


bool MemTrace::getValue(uint64_t time, uint64_t addr, uint64_t *value)
{
    bool        valueValid = false;
    uint64_t    val;

    for(auto m: memTrace){
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


