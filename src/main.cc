
#include <iostream>

#include <fst/fstapi.h>

using namespace std;

char fstName[] = "../test_data/top.fst";
//char fstName[] = "../../vexriscv_ocd_blog/tb_ocd/top.fst";
int use_vcd_extensions = 0;

string clk_scope = "TOP.top.u_vex.cpu";
string clk_value = "clk";
fstHandle clk_handle = -1;

string retired_pc_scope = "TOP.top.u_vex.cpu";
string retired_pc_value = "lastStagePc";
fstHandle retired_pc_handle = -1;

string retired_pc_valid_scope = "TOP.top.u_vex.cpu";
string retired_pc_valid_value = "lastStageIsValid";
fstHandle retired_pc_valid_handle = -1;

string fstFileTypeStrings[] = {
    "VERILOG",
    "VHDL",
    "VERILOG_VHDL"
};

string fstScopeTypeStrings[] = {
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

string fstVarTypeStrings[] = {
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

string fstVarDirStrings[] = {
    "IMPLICIT",
    "INPUT",
    "OUTPUT",
    "INOUT",
    "BUFFER",
    "LINKAGE"
};

string fstHierTypeStrings[] = {
    "SCOPE",
    "UPSCOPE",
    "VAR",
    "ATTRBEGIN",
    "ATTREND",

    /* FST_HT_TREEBEGIN and FST_HT_TREEEND are not yet used by FST but are currently used when fstHier bridges other formats */
    "TREEBEGIN",
    "TREEEND",
};


void fst_callback2(void *user_callback_data_pointer, uint64_t time, fstHandle txidx, const unsigned char *value, uint32_t len)
{
    cout << "fst_callback2:" << endl;
    (void)user_callback_data_pointer;

    cout << "    time(" << time << ")" << endl;
    cout << "    facidx(" << txidx << ")" << endl;
    cout << "    len(" << len << ")" << endl;

    for(int i=0; i< len; ++i){
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

    void *fstCtx;

    fstCtx = fstReaderOpen((const char *)fstName);
    if (!fstCtx){
        cerr << "Can't open " << fstName << endl;
        exit(-1);
    }

    fstReaderSetVcdExtensions(fstCtx, use_vcd_extensions);


    uint64_t        aliasCount                      = fstReaderGetAliasCount(fstCtx);
    const char *    curFlatScope                    = fstReaderGetCurrentFlatScope(fstCtx);
    int             curScopeLen                     = fstReaderGetCurrentScopeLen(fstCtx);
    const char *    dateStr                         = fstReaderGetDateString(fstCtx);
    int             doubleEndianMatchState          = fstReaderGetDoubleEndianMatchState(fstCtx);
    uint64_t        dumpActivityChangeTime          = fstReaderGetDumpActivityChangeTime(fstCtx, 0);
    unsigned char   dumpActivityChangeValue         = fstReaderGetDumpActivityChangeValue(fstCtx, 0);
    uint64_t        startTime                       = fstReaderGetStartTime(fstCtx);
    uint64_t        endTime                         = fstReaderGetEndTime(fstCtx);
    int64_t         timezero                        = fstReaderGetTimezero(fstCtx);
    int             fileType                        = fstReaderGetFileType(fstCtx);
    int             fseekFailed                     = fstReaderGetFseekFailed(fstCtx);
    uint64_t        memoryUsedByWriter              = fstReaderGetMemoryUsedByWriter(fstCtx);
    uint32_t        numDumpActivityChanges          = fstReaderGetNumberDumpActivityChanges(fstCtx);
    uint64_t        scopeCount                      = fstReaderGetScopeCount(fstCtx);
    signed char     timescale                       = fstReaderGetTimescale(fstCtx);
    uint64_t        valueChangeSectionCount         = fstReaderGetValueChangeSectionCount(fstCtx);
    uint64_t        varCount                        = fstReaderGetVarCount(fstCtx);
    const char *    versionStr                      = fstReaderGetVersionString(fstCtx);

//    void *          curScopeUserInfo                = fstReaderGetCurrentScopeUserInfo(fstCtx);
//    fstHandle       maxHandle                       = fstReaderGetMaxHandle(fstCtx);
//    char *          fstReaderGetValueFromHandleAtTime(fstCtx, uint64_t tim, fstHandle facidx, char *buf);
//    int             facProcessMask          = fstReaderGetFacProcessMask(fstCtx, fstHandle facidx);

    cout << "============================================================" << endl;
    cout << "aliasCount: " << aliasCount << endl;
    cout << "curFlatScope: " << curFlatScope << endl;
    cout << "curScopeLen: " << curScopeLen << endl;
    cout << "dateStr: " << dateStr << endl;
    cout << "doubleEndianMatchState: " << doubleEndianMatchState << endl;
    cout << "dumpActivityChangeTime: " << dumpActivityChangeTime << endl;
    cout << "dumpActivityChangeValue: " << (int)dumpActivityChangeValue << endl;
    cout << "fileType: " << fileType << " (" << fstFileTypeStrings[fileType] << ")" << endl;
    cout << "fseekFailed: " << fseekFailed << endl;
    cout << "memoryUsedByWriter:" << memoryUsedByWriter << endl;
    cout << "numDumpActivityChanges: " << numDumpActivityChanges << endl;
    cout << "scopeCount: " << scopeCount << endl;
    cout << "startTime: " << startTime << endl;
    cout << "endTime: " << endTime << endl;
    cout << "timeZero: " << timezero << endl;
    cout << "timescale:" << (int)timescale << endl;
    cout << "valueChangeSectionCount: " << valueChangeSectionCount << endl;
    cout << "varCount: " << varCount << endl;
    cout << "Version string: " << versionStr << endl;
    cout << "============================================================" << endl;

    if (numDumpActivityChanges > 0){
        cerr << "Blackout regions are not supported." << endl;
        exit(-2);
    }

    int numHierVars = 0;

    int hierCntr = 0;
    struct fstHier *hier;
    string cur_scope_name; 
    while((hier = fstReaderIterateHier(fstCtx))){
        cout << "hierType(" << fstHierTypeStrings[hier->htyp] << "), hierCntr(" << hierCntr << ")" << endl;

        switch(hier->htyp){
            case FST_HT_SCOPE: {
                cout << "    scope.typ(" << (int)hier->u.scope.typ << "," << fstScopeTypeStrings[hier->u.scope.typ] << ")" << endl;
                cout << "    scope.name(" << hier->u.scope.name << ")" << endl;
                cout << "    scope.component(" << hier->u.scope.component << ")" << endl;

                const char *scope_full_name     = fstReaderPushScope(fstCtx, hier->u.scope.name, NULL);
                int         scope_full_name_len = fstReaderGetCurrentScopeLen(fstCtx);
                cout << "    scope_full_name(" << scope_full_name << ")" << endl;
                cout << "    scope_full_name_len(" << scope_full_name_len << ")" << endl;

                cur_scope_name  = scope_full_name;
                
                break;
            }

            case FST_HT_UPSCOPE: {
                const char *scope_full_name     = fstReaderPopScope(fstCtx);
                int         scope_full_name_len = fstReaderGetCurrentScopeLen(fstCtx);
                cout << "    scope_full_name(" << scope_full_name << ")" << endl;
                cout << "    scope_full_name_len(" << scope_full_name_len << ")" << endl;

                cur_scope_name  = scope_full_name;
                
                break;
            }

            case FST_HT_VAR: {
                cout << "    var.name(" << hier->u.var.name << ")" << endl;
                cout << "    var.scope(" << cur_scope_name << ")" << endl;
                cout << "    var.typ(" << (int)hier->u.var.typ << "," << fstVarTypeStrings[hier->u.var.typ] << ")" << endl;
                cout << "    var.direction(" << (int)hier->u.var.direction << "," << fstVarDirStrings[hier->u.var.direction] << ")" << endl;
                cout << "    var.length(" << hier->u.var.length << ")" << endl;
                cout << "    var.is_alias(" << hier->u.var.is_alias << ")" << endl;
                cout << "    var.handle(" << hier->u.var.handle << ")" << endl;
                cout << "    numHierVars(" << numHierVars << ")" << endl;

                if (clk_scope == cur_scope_name && clk_value == hier->u.var.name){
                    cout << "CLK MATCH!!!" << endl;
                    clk_handle   = hier->u.var.handle;
                }

                if (retired_pc_scope == cur_scope_name && retired_pc_value == hier->u.var.name){
                    cout << "PC MATCH!!!" << endl;
                    retired_pc_handle   = hier->u.var.handle;
                }

                if (retired_pc_valid_scope == cur_scope_name && retired_pc_valid_value == hier->u.var.name){
                    cout << "PC_VALID MATCH!!!" << endl;
                    retired_pc_valid_handle   = hier->u.var.handle;
                }

                ++numHierVars;
                break;
            }
        }

        ++hierCntr;

    }

#if 0
    for(int varIdx=0; varIdx<varCount; ++varIdx){
        cout << "varIdx(" << varIdx << "/" << varCount << ")" << endl;
    }
#endif

//    fstReaderSetFacProcessMaskAll(fstCtx);
    fstReaderSetFacProcessMask(fstCtx, clk_handle);
    fstReaderSetFacProcessMask(fstCtx, retired_pc_valid_handle);
    fstReaderSetFacProcessMask(fstCtx, retired_pc_handle);
    fstReaderIterBlocks2(fstCtx, fst_callback, fst_callback2, fstCtx, NULL); 


    return 0;
}
