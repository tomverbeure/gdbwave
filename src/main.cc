
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <fst/fstapi.h>

#include "Logger.h"
#include "CpuTrace.h"
#include "MemTrace.h"
#include "RegFileTrace.h"
#include "FstProcess.h"
#include "TcpServer.h"
#include "gdbstub.h"

#define DEVELOP   1

using namespace std;

bool verbose = false;

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
    LOG_INFO("Usage: gdbwave <options>");
    LOG_INFO("    -w <FST waveform file>");
    LOG_INFO("    -c <config parameter file>");
    LOG_INFO("    -p <port nr>");
    LOG_INFO("    -v verbose");
    LOG_INFO("");
    LOG_INFO("Example: ./gdbwave -w ./test_data/top.fst -c ./test_data/configParams.txt");
    LOG_INFO("");
}

void parseConfig(ifstream & configFile, ConfigParams &c)
{
    string line;
    LOG_INFO("Reading configuration parameters...");

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
            LOG_ERROR("Unknown configuration parameter: %s", name.c_str());
            exit(1);
        }

        LOG_INFO("%s:%s", name.c_str(), value.c_str());
    }
}

int main(int argc, char **argv)
{
    int c;
    Logger::log().setDebugLevel(Logger::DEBUG);
    Logger::log().setLogFile("./gdbwave.log");

    ConfigParams configParams;
    int portNr = 3333;

    string fstFileName; 
    string configParamsFileName;

    while((c = getopt(argc, argv, "hw:c:p:v")) != -1){
        switch(c){
            case 'h':
                help();
                exit(0);
                break;
            case 'w': 
                fstFileName = optarg;
                break;
            case 'c':
                configParamsFileName = optarg;
                break;
            case 'p':
                portNr = stoi(optarg);
                break;
            case 'v':
                verbose = true;
                break;
            case '?':
                return 1;
        }
    }

#if DEVELOP==1
    if (fstFileName.empty()){
        fstFileName             = "../test_data/top.fst";
    }
    if (configParamsFileName.empty()){
        configParamsFileName    = "../test_data/configParams.txt";
    }
#else
    if (fstFileName.empty()){
        LOG_ERROR("FST waveform file not specified!");
        return 1;
    }

    if (configParamsFileName.empty()){
        LOG_ERROR(stderr, "Configuration parameter file not specified!");
        return 1;
    }
#endif

    ifstream configFile(configParamsFileName, ios::in);
    parseConfig(configFile, configParams);

    if (configParams.cpuClkSignal.empty()){
        LOG_ERROR("CPU clock signal not specified!");
        return 1;
    }

    if (configParams.retiredPcSignal.empty()){
        LOG_ERROR("CPU program counter signal not specified!");
        return 1;
    }

    if (configParams.retiredPcValidSignal.empty()){
        LOG_ERROR("CPU program counter valid signal not specified!");
        return 1;
    }

    FstProcess  fstProc(fstFileName);
    LOG_INFO("%s", fstProc.infoStr().c_str());

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

    TcpServer tcpServer(portNr);
    dbg_sys_init(tcpServer, cpuTrace, regFileTrace, memTrace);

    return 0;
}
