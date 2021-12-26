
#include <unistd.h>
#include <iostream>
#include <string>

#include <fst/fstapi.h>

#include "CpuTrace.h"
#include "MemTrace.h"
#include "FstProcess.h"
#include "TcpServer.h"
#include "gdbstub.h"

using namespace std;

#if 0
void fst_callback2(void *user_callback_data_pointer, uint64_t time, fstHandle txidx, const unsigned char *value, uint32_t len)
{
    cout << "fst_callback2:" << endl;
    (void)user_callback_data_pointer;

    cout << "    time(" << time << ")" << endl;
    cout << "    facidx(" << txidx << ")" << endl;
    cout << "    len(" << len << ")" << endl;

    for(uint32_t i=0; i< len; ++i){
    }
}

void fst_callback(void *user_callback_data_pointer, uint64_t time, fstHandle txidx, const unsigned char *value)
{
//    fst_callback2(user_callback_data_pointer, tim, txidx, value, 0);
    cout << "fst_callback:" << endl;
    (void)user_callback_data_pointer;

    cout << "    time(" << time << ")" << endl;
    cout << "    facidx(" << txidx << ")" << endl;
    cout << "    len(" << strlen((const char *)value) << ")" << endl;
    cout << "    value(" << value << ")" << endl;
}
#endif

void gdb_proc(TcpServer &tcpServer, CpuTrace &cpuTrace, MemTrace &regFileTrace)
{
    dbg_sys_init(tcpServer, cpuTrace, regFileTrace);
}

string get_scope(string full_path)
{
    int last_dot = full_path.find_last_of('.');

    return full_path.substr(0, last_dot);
}

string get_local_name(string full_path)
{
    int last_dot = full_path.find_last_of('.');

    return full_path.substr(last_dot+1);
}

void help()
{
    fprintf(stderr, "Usage: gdbwave <options>\n");
    fprintf(stderr, "    -w <FST waveform file>\n");
    fprintf(stderr, "    -c <hierachical signal of the CPU clock>\n");
    fprintf(stderr, "    -p <hierachical signal of the CPU program counter>\n");
    fprintf(stderr, "    -e <hierachical signal of the CPU program counter valid>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Example: ./gdbwave -w ./test_data/top.fst -c TOP.top.u_vex.cpu -p TOP.top.u_vex.cpu.lastStagePc -e TOP.top.u_vex.cpu.lastStageIsValid\n");
    fprintf(stderr, "\n");
}

int main(int argc, char **argv)
{
    int c;

    string fstFileName; 
    string cpuClkSignal;
    string retiredPcSignal;
    string retiredPcValidSignal;

    string regFileWriteValidSignal = "TOP.top.u_vex.cpu.lastStageRegFileWrite_valid";
    string regFileWriteAddrSignal  = "TOP.top.u_vex.cpu.lastStageRegFileWrite_payload_address";
    string regFileWriteDataSignal  = "TOP.top.u_vex.cpu.lastStageRegFileWrite_payload_data";

    // FIXME: eventually, switch to getopt_long?
    while((c = getopt(argc, argv, "hw:c:p:e:")) != -1){
        switch(c){
            case 'h':
                help();
                break;
            case 'w': 
                fstFileName = optarg;
                break;
            case 'c':
                cpuClkSignal = optarg;
                break;
            case 'p':
                retiredPcSignal = optarg;
                break;
            case 'e':
                retiredPcValidSignal = optarg;
                break;
            case '?':
                return 1;
        }
    }

#if 0
    if (fstFileName.empty()){
        fprintf(stderr, "FST waveform file not specified!\n\n");
        return 1;
    }

    if (cpuClkSignal.empty()){
        fprintf(stderr, "CPU clock signal not specified!\n\n");
        return 1;
    }

    if (retiredPcSignal.empty()){
        fprintf(stderr, "CPU program counter signal not specified!\n\n");
        return 1;
    }

    if (retiredPcValidSignal.empty()){
        fprintf(stderr, "CPU program counter valid signal not specified!\n\n");
        return 1;
    }
#else
    if (fstFileName.empty()){
        fstFileName             = "../test_data/top.fst";
    }

    if (cpuClkSignal.empty()){
        cpuClkSignal            = "TOP.top.u_vex.cpu.clk";
    }

    if (retiredPcSignal.empty()){
        retiredPcSignal         = "TOP.top.u_vex.cpu.lastStagePc";
    }

    if (retiredPcValidSignal.empty()){
        retiredPcValidSignal    = "TOP.top.u_vex.cpu.lastStageIsValid";
    }
#endif

    string cpuClkScope = get_scope(cpuClkSignal);
    string cpuClkName  = get_local_name(cpuClkSignal);

    string retiredPcScope = get_scope(retiredPcSignal);
    string retiredPcName  = get_local_name(retiredPcSignal);

    string retiredPcValidScope = get_scope(retiredPcValidSignal);
    string retiredPcValidName  = get_local_name(retiredPcValidSignal);

    string regFileWriteValidScope   = get_scope(regFileWriteValidSignal);
    string regFileWriteValidName    = get_local_name(regFileWriteValidSignal);

    string regFileWriteAddrScope    = get_scope(regFileWriteAddrSignal);
    string regFileWriteAddrName     = get_local_name(regFileWriteAddrSignal);

    string regFileWriteDataScope    = get_scope(regFileWriteDataSignal);
    string regFileWriteDataName     = get_local_name(regFileWriteDataSignal);

    FstProcess  fstProc(fstFileName);
    cout << fstProc.infoStr();

    FstSignal clkSig(cpuClkScope, cpuClkName);

    FstSignal regFileWriteValidSig(regFileWriteValidScope, regFileWriteValidName);
    FstSignal regFileWriteAddrSig (regFileWriteAddrScope,  regFileWriteAddrName);
    FstSignal regFileWriteDataSig (regFileWriteDataScope,  regFileWriteDataName);

    MemTrace    regFileTrace(fstProc, clkSig, regFileWriteValidSig, regFileWriteAddrSig, regFileWriteDataSig);

    FstSignal retiredPcSig(retiredPcScope, retiredPcName);
    FstSignal retiredPcValidSig(retiredPcValidScope, retiredPcValidName);

    CpuTrace    cpuTrace(fstProc, clkSig, retiredPcValidSig, retiredPcSig);

    while(1){
        TcpServer tcpServer(3333);
        gdb_proc(tcpServer, cpuTrace, regFileTrace);
    }

#if 0
    unsigned char rxbuf[256];
    int ret;

    while( (ret = tcpServer.recv(rxbuf, 256)) > 0){
        cout << "ret:" <<  ret << endl;
        if (ret > 0){
            tcpServer.xmit("hhh", 3);
            tcpServer.xmit(rxbuf, ret);
        }
    }

    if (ret == 0){
        cout << "Connection closed..." << endl;
    }
    else{
        cout << "Connection error: " << ret << endl;
    }
#endif

    return 0;
}
