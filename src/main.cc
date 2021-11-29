
#include <iostream>

#include <fst/fstapi.h>

#include "CpuTrace.h"
#include "FstProcess.h"
#include "TcpServer.h"

using namespace std;

char fstFileName[] = "../test_data/top.fst";
//char fstFileName[] = "../../vexriscv_ocd_blog/tb_ocd/top.fst";

string clk_scope = "TOP.top.u_vex.cpu";
string clk_name = "clk";
fstHandle clk_handle = -1;

string retired_pc_scope = "TOP.top.u_vex.cpu";
string retired_pc_name = "lastStagePc";
fstHandle retired_pc_handle = -1;

string retired_pc_valid_scope = "TOP.top.u_vex.cpu";
string retired_pc_valid_name = "lastStageIsValid";
fstHandle retired_pc_valid_handle = -1;

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

int main(int argc, char **argv)
{
    FstProcess  fstProc(fstFileName);

    cout << fstProc.infoStr();

    FstSignal clk_sig(clk_scope, clk_name);
    FstSignal retired_pc_sig(retired_pc_scope, retired_pc_name);
    FstSignal retired_pc_valid_sig(retired_pc_valid_scope, retired_pc_valid_name);

    CpuTrace    cpuTrace(fstProc, clk_sig, retired_pc_valid_sig, retired_pc_sig);

    TcpServer tcpServer(3333);

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

    return 0;
}
