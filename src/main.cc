
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <fst/fstapi.h>

#include "CpuTrace.h"
#include "MemTrace.h"
#include "RegFileTrace.h"
#include "FstProcess.h"
#include "TcpServer.h"
#include "gdbstub.h"

#define DEBUG   1

using namespace std;

struct ConfigParams {
    string fstFileName; 
    string cpuClkSignal;
    string retiredPcSignal;
    string retiredPcValidSignal;

    string regFileWriteValidSignal;
    string regFileWriteAddrSignal;
    string regFileWriteDataSignal;

    string memCmdValidSignal;
    string memCmdReadySignal;
    string memCmdAddrSignal;
    string memCmdSizeSignal;
    string memCmdWrSignal;
    string memCmdWrDataSignal;
    string memRspValidSignal;
    string memRspRdDataSignal;

    string memInitFileName;
    int memInitStartAddr;
};

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

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

void help()
{
    fprintf(stderr, "Usage: gdbwave <options>\n");
    fprintf(stderr, "    -w <FST waveform file>\n");
    fprintf(stderr, "    -c <config parameter file>\n");
    fprintf(stderr, "    -v verbose\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Example: ./gdbwave -w ./test_data/top.fst -c ./test_data/configParams.txt\n");
    fprintf(stderr, "\n");
}

void parseConfig(ifstream & configFile, ConfigParams &c)
{
    string line;
    cout << "Reading configuration parameters..." << endl;

    while(getline(configFile, line)){
        trim(line);
        if (line[0] == '#' || line.empty())
            continue;
        auto delimiterPos   = line.find("=");
        auto name           = line.substr(0, delimiterPos);
        auto value          = line.substr(delimiterPos+1);

        trim(name);
        trim(value);

        if (name == "cpuClk")
            c.cpuClkSignal                  = value;
        else if (name == "retiredPc")
            c.retiredPcSignal               = value;
        else if (name == "retiredPcValid")
            c.retiredPcValidSignal          = value;
        else if (name == "regFileWriteValid")
            c.regFileWriteValidSignal       = value;
        else if (name == "regFileWriteAddr")
            c.regFileWriteAddrSignal        = value;
        else if (name == "regFileWriteData")
            c.regFileWriteDataSignal        = value;

        else if (name == "memCmdValid")
            c.memCmdValidSignal             = value;
        else if (name == "memCmdReady")
            c.memCmdReadySignal             = value;
        else if (name == "memCmdAddr")
            c.memCmdAddrSignal              = value;
        else if (name == "memCmdSize")
            c.memCmdSizeSignal              = value;
        else if (name == "memCmdWr")
            c.memCmdWrSignal                = value;
        else if (name == "memCmdWrData")
            c.memCmdWrDataSignal            = value;
        else if (name == "memRspValid")
            c.memRspValidSignal             = value;
        else if (name == "memRspRdData")
            c.memRspRdDataSignal            = value;

        else if (name == "memInitFile")
            c.memInitFileName               = value;
        else if (name == "memInitStartAddr")
            c.memInitStartAddr              = stoi(value);

        else{
            cerr << "Unknown configuration parameter: " << name << endl;
            exit(1);
        }

        cout << name << ":" << value << endl;
    }
}

int main(int argc, char **argv)
{
    int c;

    ConfigParams configParams;


    string fstFileName; 
    string configParamsFileName;

    while((c = getopt(argc, argv, "hw:c:v")) != -1){
        switch(c){
            case 'h':
                help();
                break;
            case 'w': 
                fstFileName = optarg;
                break;
            case 'c':
                configParamsFileName = optarg;
                break;
            case '?':
                return 1;
        }
    }

#if DEBUG==1
    if (fstFileName.empty()){
        fstFileName             = "../test_data/top.fst";
    }
    if (configParamsFileName.empty()){
        configParamsFileName    = "../test_data/configParams.txt";
    }
#else
    if (fstFileName.empty()){
        fprintf(stderr, "FST waveform file not specified!\n\n");
        return 1;
    }

    if (configParamsFileName.empty()){
        fprintf(stderr, "Configuration parameter file not specified!\n\n");
        return 1;
    }
#endif

    ifstream configFile(configParamsFileName, ios::in);
    parseConfig(configFile, configParams);

    if (configParams.cpuClkSignal.empty()){
        fprintf(stderr, "CPU clock signal not specified!\n\n");
        return 1;
    }

    if (configParams.retiredPcSignal.empty()){
        fprintf(stderr, "CPU program counter signal not specified!\n\n");
        return 1;
    }

    if (configParams.retiredPcValidSignal.empty()){
        fprintf(stderr, "CPU program counter valid signal not specified!\n\n");
        return 1;
    }

    FstProcess  fstProc(fstFileName);
    cout << fstProc.infoStr();

    FstSignal clkSig(configParams.cpuClkSignal);

    FstSignal retiredPcSig     (configParams.retiredPcSignal);
    FstSignal retiredPcValidSig(configParams.retiredPcValidSignal);

    FstSignal regFileWriteValidSig(configParams.regFileWriteValidSignal);
    FstSignal regFileWriteAddrSig (configParams.regFileWriteAddrSignal);
    FstSignal regFileWriteDataSig (configParams.regFileWriteDataSignal);

    FstSignal memCmdValidSig   (configParams.memCmdValidSignal);
    FstSignal memCmdReadySig   (configParams.memCmdReadySignal);
    FstSignal memCmdAddrSig    (configParams.memCmdAddrSignal);
    FstSignal memCmdSizeSig    (configParams.memCmdSizeSignal);
    FstSignal memCmdWrSig      (configParams.memCmdWrSignal);
    FstSignal memCmdWrDataSig  (configParams.memCmdWrDataSignal);
    FstSignal memRspValidSig   (configParams.memRspValidSignal);
    FstSignal memRspDataSig    (configParams.memRspRdDataSignal);

    CpuTrace        cpuTrace(fstProc, clkSig, retiredPcValidSig, retiredPcSig);
    RegFileTrace    regFileTrace(fstProc, clkSig, regFileWriteValidSig, regFileWriteAddrSig, regFileWriteDataSig);
    MemTrace        memTrace(fstProc, 
                             configParams.memInitFileName, configParams.memInitStartAddr,
                             clkSig, 
                             memCmdValidSig, memCmdReadySig, memCmdAddrSig, memCmdSizeSig, memCmdWrSig, memCmdWrDataSig, 
                             memRspValidSig, memRspDataSig);

    while(1){
        TcpServer tcpServer(3333);
        dbg_sys_init(tcpServer, cpuTrace, regFileTrace, memTrace);
    }

    return 0;
}
