
#include <exception>
#include <sstream>

#include "FstProcess.h"
#include "Logger.h"

FstProcess::FstProcess(string fstFileName) :
    fstFileName(fstFileName)
{
    fstCtx = fstReaderOpen(fstFileName.c_str());
    if (fstCtx == NULL) {
        stringstream ss;
        ss << "Could not open file '" << fstFileName << "'";
        throw runtime_error(ss.str());
    }
}

FstProcess::~FstProcess()
{
    fstReaderClose(fstCtx);
}

string FstProcess::infoStr(void)
{
    stringstream ss;

    const char *    curFlatScope                    = fstReaderGetCurrentFlatScope(fstCtx);
    int             curScopeLen                     = fstReaderGetCurrentScopeLen(fstCtx);
    int             doubleEndianMatchState          = fstReaderGetDoubleEndianMatchState(fstCtx);
    uint64_t        dumpActivityChangeTime          = fstReaderGetDumpActivityChangeTime(fstCtx, 0);
    unsigned char   dumpActivityChangeValue         = fstReaderGetDumpActivityChangeValue(fstCtx, 0);
    int             fileType                        = fstReaderGetFileType(fstCtx);
    int             fseekFailed                     = fstReaderGetFseekFailed(fstCtx);
    uint64_t        memoryUsedByWriter              = fstReaderGetMemoryUsedByWriter(fstCtx);
    uint32_t        numDumpActivityChanges          = fstReaderGetNumberDumpActivityChanges(fstCtx);

//    void *          curScopeUserInfo                = fstReaderGetCurrentScopeUserInfo(fstCtx);
//    fstHandle       maxHandle                       = fstReaderGetMaxHandle(fstCtx);
//    char *          fstReaderGetValueFromHandleAtTime(fstCtx, uint64_t tim, fstHandle facidx, char *buf);
//    int             facProcessMask          = fstReaderGetFacProcessMask(fstCtx, fstHandle facidx);

    ss << "============================================================" << endl;
    ss << "aliasCount: " << aliasCount() << endl;
    ss << "curFlatScope: " << curFlatScope << endl;
    ss << "curScopeLen: " << curScopeLen << endl;
    ss << "date: " << date() << endl;
    ss << "doubleEndianMatchState: " << doubleEndianMatchState << endl;
    ss << "dumpActivityChangeTime: " << dumpActivityChangeTime << endl;
    ss << "dumpActivityChangeValue: " << (int)dumpActivityChangeValue << endl;
    ss << "fileType: " << fileType << " (" << fileTypeStrings[fileType] << ")" << endl;
    ss << "fseekFailed: " << fseekFailed << endl;
    ss << "memoryUsedByWriter:" << memoryUsedByWriter << endl;
    ss << "numDumpActivityChanges: " << numDumpActivityChanges << endl;
    ss << "scopeCount: " << scopeCount() << endl;
    ss << "startTime: " << startTime() << endl;
    ss << "endTime: " << endTime() << endl;
    ss << "timeZero: " << timezero() << endl;
    ss << "timescale:" << timescale() << endl;
    ss << "valueChangeSectionCount: " << valueChangeSectionCount() << endl;
    ss << "varCount: " << varCount() << endl;
    ss << "Version string: " << version() << endl;
    ss << "============================================================" << endl;

    // FIXME: move...
    if (numDumpActivityChanges > 0){
        LOG_ERROR("Blackout regions are not supported.");
        exit(-2);
    }

    return ss.str();
}

// For a given list of signals, look for and assign the fstHandle
// Return true if all signals are found. 
bool FstProcess::assignHandles(vector<FstSignal *> &signals)
{
    struct fstHier *hier;
    string curScopeName; 

    bool sigNotFound;

    fstReaderIterateHierRewind(fstCtx);
    while((hier = fstReaderIterateHier(fstCtx))){

        switch(hier->htyp){
            case FST_HT_SCOPE: {
                curScopeName = fstReaderPushScope(fstCtx, hier->u.scope.name, NULL);
                LOG_DEBUG("curScopeName: %s", curScopeName.c_str());
                break;
            }

            case FST_HT_UPSCOPE: {
                curScopeName = fstReaderPopScope(fstCtx);
                LOG_DEBUG("curScopeName: %s", curScopeName.c_str());
                break;
            }

            case FST_HT_VAR: {
                string curName = hier->u.var.name;
                LOG_DEBUG("curName: %s", curName.c_str());

                sigNotFound = false;
                for(auto sig: signals){
                    if (sig->hasHandle)
                        continue;
                    sigNotFound     = true;

                    if (sig->scopeName == curScopeName && sig->name == curName){
                        sig->hasHandle   = true;
                        sig->handle      = hier->u.var.handle;
                    }
                }
                break;
            }
        }

    }

    return !sigNotFound;
}

void FstProcess::reportSignalsNotFound(vector<FstSignal *> &signals)
{
    LOG_WARNING("Not all signals found...");

    for(auto sig: signals){
        if (!sig->hasHandle){
            LOG_WARNING("Signal not found: %s.%s\n", sig->scopeName.c_str(), sig->name.c_str());
        }
    }
}

void FstProcess::fst_callback2(void *user_callback_data_pointer, uint64_t time, fstHandle sigHandle, const unsigned char *value, uint32_t len)
{
    throw runtime_error("callback2 not supported.");
}

void FstProcess::fst_callback(void *user_callback_data_pointer, uint64_t time, fstHandle sigHandle, const unsigned char *value)
{
    struct FstCallbackInfo *cbInfo = (struct FstCallbackInfo *)user_callback_data_pointer;
    map<fstHandle,FstSignal *>::iterator it = cbInfo->handle2Signal.find(sigHandle);

    cbInfo->valueChangedCB(time, it->second, value, cbInfo->userInfo);
}

void FstProcess::getValueChanges(
                    vector<FstSignal *> signals, 
                    void (*valueChangedCB)(uint64_t time, FstSignal *signal, const unsigned char *value, void *userInfo), 
                    void *userInfo)
{
    struct FstCallbackInfo cbInfo;
    cbInfo.valueChangedCB   = valueChangedCB;
    cbInfo.userInfo         = userInfo;

    fstReaderClrFacProcessMaskAll(fstCtx);

    for(auto sig: signals){
        fstReaderSetFacProcessMask(fstCtx, sig->handle);
        cbInfo.handle2Signal.insert({sig->handle, sig});
    }

    fstReaderIterBlocks2(fstCtx, FstProcess::fst_callback, FstProcess::fst_callback2, (void *)&cbInfo, NULL); 
}

