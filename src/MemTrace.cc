
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#include "MemTrace.h"

MemTrace::MemTrace(FstProcess & fstProc, string memInitFileName, int memInitStartAddr,  
                FstSignal clk, 
                FstSignal memCmdValid, FstSignal memCmdReady, FstSignal memCmdAddr, FstSignal memCmdSize, FstSignal memCmdWr, FstSignal memCmdWrData,
                FstSignal memRspValid, FstSignal memRspData) :
    fstProc(fstProc), 
    memInitFileName(memInitFileName),
    memInitStartAddr(memInitStartAddr),
    clk(clk),
    memCmdValid(memCmdValid),
    memCmdReady(memCmdReady),
    memCmdAddr(memCmdAddr),
    memCmdSize(memCmdSize),
    memCmdWr(memCmdWr),
    memCmdWrData(memCmdWrData),
    memRspValid(memRspValid),
    memRspData(memRspData)
{
    init();
}

static void memChangedCB(uint64_t time, FstSignal *signal, const unsigned char *value, void *userInfo)
{
    MemTrace *memTrace = (MemTrace *)userInfo;
    memTrace->processSignalChanged(time, signal, value);
}

void MemTrace::processSignalChanged(uint64_t time, FstSignal *signal, const unsigned char *value)
{
    uint64_t valueInt = stol(string((const char *)value), nullptr, 2);
    //cout << time << "," << signal->handle << "," << signal->name << "," << value << "," << valueInt << endl;

    if (signal->handle == memCmdValid.handle){
        curMemCmdValid      = valueInt;
        return;
    }

    if (signal->handle == memCmdReady.handle){
        curMemCmdReady      = valueInt;
        return;
    }

    if (signal->handle == memCmdAddr.handle){
        curMemCmdAddr       = valueInt;
        return;
    }

    if (signal->handle == memCmdSize.handle){
        curMemCmdSize       = valueInt;
        return;
    }

    if (signal->handle == memCmdWr.handle){
        curMemCmdWr         = valueInt;
        return;
    }

    if (signal->handle == memCmdWrData.handle){
        curMemCmdWrData     = valueInt;
        return;
    }

    if (signal->handle == memRspValid.handle){
        curMemRspValid      = valueInt;
        return;
    }

    if (signal->handle == memRspData.handle){
        curMemRspData       = valueInt;
        return;
    }

    // All signals changes on the rising edge of the clock. Everything is stable at the falling edge...
    if (signal->handle == clk.handle && valueInt == 0){
        if (curMemCmdValid && curMemCmdReady){
            // For now, only handle memory writes.
            if (curMemCmdWr){
                int byteEna = 0;
                switch(curMemCmdSize){
                    case 0:  byteEna     = 1 << (curMemCmdAddr & 3); break;
                    case 1:  byteEna     = 3 << (curMemCmdAddr & 3); break;
                    default: byteEna     = 15; break;
                }

                for(int byteNr=0; byteNr<4;++byteNr){
                    if (byteEna & (1<<byteNr)){
                        uint64_t byteVal    = (curMemCmdWrData >> (byteNr * 8)) & 255;
                        uint64_t addr       = (curMemCmdAddr & ~3) | byteNr;

                        printf("MemWr: 0x%08lx <- 0x%02lx (@%ld)\n", addr, byteVal, time);

                        MemAccess   ma = { time, curMemCmdWr, addr, byteVal }; 
                        memTrace.push_back(ma);
                    }
                }
            }
        }
    }
}

void MemTrace::init()
{
    if (!memInitFileName.empty()){
        printf("Loading mem init file: %s\n", memInitFileName.c_str());
        ifstream initFile(memInitFileName, ios::in | ios::binary);
        if (initFile.fail()){
            cerr << "Error opening mem init file: " << memInitFileName << " (" << strerror(errno) << ")" << endl;
            exit(1);
        }
        memInitContents = vector<char>((std::istreambuf_iterator<char>(initFile)), std::istreambuf_iterator<char>());
    }

    vector<FstSignal *> sigs;

    sigs.push_back(&clk);
    sigs.push_back(&memCmdValid);
    sigs.push_back(&memCmdReady);
    sigs.push_back(&memCmdAddr);
    sigs.push_back(&memCmdSize);
    sigs.push_back(&memCmdWr);
    sigs.push_back(&memCmdWrData);
    sigs.push_back(&memRspValid);
    sigs.push_back(&memRspData);

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

    curMemCmdValid  = false;
    curMemCmdReady  = false;
    curMemCmdAddr   = 0;
    curMemCmdSize   = 0;
    curMemCmdWr     = false;
    curMemCmdWrData = 0;
    curMemRspValid  = false;
    curMemRspData   = 0;

    fstProc.getValueChanges(sigs, memChangedCB, (void *)this);

    printf("Nr mem transactions: %ld\n", memTrace.size());
}


bool MemTrace::getValue(uint64_t time, uint64_t addr, char *value)
{
    bool        valueValid = false;
    uint64_t    val = 0;

    if (addr >= memInitStartAddr && addr < (memInitStartAddr + memInitContents.size())){
        valueValid = true;
        val = memInitContents[addr - memInitStartAddr];
    }

    for(auto m: memTrace){
        if (m.time > time)
            break;

        if (m.wr && m.addr == addr){
            valueValid      = true;
            val             = m.value;
        }
    }

    *value      = val;

    return valueValid;
}



