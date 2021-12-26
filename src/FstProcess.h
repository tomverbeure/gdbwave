#ifndef FST_PROCESS_H
#define FST_PROCESS_H

#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <map>

#include <fst/fstapi.h>

using namespace std;

class FstSignal
{
public:
    FstSignal() : hasHandle(false) {}
    FstSignal(string scopeName, string name) : scopeName(scopeName), name(name), hasHandle(false) {}
    FstSignal(string fullName) : scopeName(getScope(fullName)), name(getLocalName(fullName)), hasHandle(false) {}

    string      scopeName;
    string      name;
    bool        hasHandle;
    fstHandle   handle;

    static string getScope(string full_path){ 
        int last_dot = full_path.find_last_of('.');
        return full_path.substr(0, last_dot);
    }
    static string getLocalName(string full_path){
        int last_dot = full_path.find_last_of('.');
        return full_path.substr(last_dot+1);
    }
};

class FstProcess
{
public:
    FstProcess(string fstFileName);
    ~FstProcess();

    string      version(void)       { return fstReaderGetVersionString(fstCtx); };
    string      date(void)          { 
        string d = fstReaderGetDateString(fstCtx);
        d = regex_replace(d, regex("^\\s+"), string(""));
        d = regex_replace(d, regex("\\s+$"), string(""));
        return d;
    }

    uint64_t        aliasCount(void)    { return fstReaderGetAliasCount(fstCtx); };
    uint64_t        startTime(void)     { return fstReaderGetStartTime(fstCtx); };
    uint64_t        endTime(void)       { return fstReaderGetEndTime(fstCtx); };
    int64_t         timezero(void)      { return fstReaderGetTimezero(fstCtx); };
    uint64_t        scopeCount(void)    { return fstReaderGetScopeCount(fstCtx); };
    int             timescale(void)     { return (int)fstReaderGetTimescale(fstCtx); };
    uint64_t        varCount(void)      { return fstReaderGetVarCount(fstCtx); };
    uint64_t        valueChangeSectionCount(void) { return fstReaderGetValueChangeSectionCount(fstCtx); };

    string infoStr(void);

    bool assignHandles(vector<FstSignal *> &signals);
    void getValueChanges(vector<FstSignal *> signals, void (*valueChangedCB)(uint64_t time, FstSignal *signal, const unsigned char *value, void *userInfo), void *userInfo);

    struct FstCallbackInfo {
        map<fstHandle,FstSignal *> handle2Signal;
        void (*valueChangedCB)(uint64_t time, FstSignal *signal, const unsigned char *value, void *userInfo);
        void *userInfo;
    };

    static void fst_callback2(void *user_callback_data_pointer, uint64_t time, fstHandle txidx, const unsigned char *value, uint32_t len);
    static void fst_callback(void *user_callback_data_pointer, uint64_t time, fstHandle txidx, const unsigned char *value);

//private:
    string  fstFileName;
    void *  fstCtx;
    
public:
    const vector<string> fileTypeStrings = {
        "VERILOG",
        "VHDL",
        "VERILOG_VHDL"
    };

    const vector<string> scopeTypeStrings = {
        "VCD_MODULE",
        "VCD_TASK",
        "VCD_FUNCTION",
        "VCD_BEGIN",
        "VCD_FORK",
        "VCD_GENERATE",
        "VCD_STRUCT",
        "VCD_UNION",
        "VCD_CLASS",
        "VCD_INTERFACE",
        "VCD_PACKAGE",
        "VCD_PROGRAM",
    
        "VHDL_ARCHITECTURE",
        "VHDL_PROCEDURE",
        "VHDL_FUNCTION",
        "VHDL_RECORD",
        "VHDL_PROCESS",
        "VHDL_BLOCK",
        "VHDL_FOR_GENERATE",
        "VHDL_IF_GENERATE",
        "VHDL_GENERATE",
        "VHDL_PACKAGE"
    };
    
    const vector<string> varTypeStrings = {
        "VCD_EVENT",
        "VCD_INTEGER",
        "VCD_PARAMETER",
        "VCD_REAL",
        "VCD_REAL_PARAMETER",
        "VCD_REG",
        "VCD_SUPPLY0",
        "VCD_SUPPLY1",
        "VCD_TIME",
        "VCD_TRI",
        "VCD_TRIAND",
        "VCD_TRIOR",
        "VCD_TRIREG",
        "VCD_TRI0",
        "VCD_TRI1",
        "VCD_WAND",
        "VCD_WIRE",
        "VCD_WOR",
        "VCD_PORT",
        "VCD_SPARRAY",
        "VCD_REALTIME",
    
        "GEN_STRING",
    
        "SV_BIT",
        "SV_LOGIC",
        "SV_INT",
        "SV_SHORTINT",
        "SV_LONGINT",
        "SV_BYTE",
        "SV_ENUM",
        "SV_SHORTREAL"
    };
    
    const vector<string> varDirStrings = {
        "IMPLICIT",
        "INPUT",
        "OUTPUT",
        "INOUT",
        "BUFFER",
        "LINKAGE"
    };
    
    const vector<string> hierTypeStrings = {
        "SCOPE",
        "UPSCOPE",
        "VAR",
        "ATTRBEGIN",
        "ATTREND",
    
        "TREEBEGIN",
        "TREEEND",
    };

};

#endif

