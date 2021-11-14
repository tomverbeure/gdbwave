
#include <iostream>

using namespace std;

#include "CpuTrace.h"

CpuTrace::CpuTrace(FstProcess & fstProc, FstSignal clk, FstSignal pcValid, FstSignal pc) :
    fstProc(fstProc), 
    clk(clk),
    pcValid(pcValid),
    pc(pc)
{
    vector<FstSignal> sigs;

    sigs.push_back(clk);
    sigs.push_back(pcValid);
    sigs.push_back(pc);

    bool allSigsFound = fstProc.assignHandles(sigs);
    if (!allSigsFound){
        cout << "Not all signals found..." << endl;
        exit(-1);
    }

    for(auto sig : sigs){
        cout << sig.scopeName << "," << sig.name << "," << sig.hasHandle << "," << sig.handle << endl;
    }
}

void CpuTrace::findNextPcValue(uint64_t startTime, uint64_t pc_value)
{
}
