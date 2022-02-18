
#include <cstdio>
#include <map>

#include "TcpServer.h"
#include "Logger.h"

#include "gdbstub.h"
#include "gdbstub_sys.h"

static TcpServer    *tcpServer;
static CpuTrace     *cpuTrace;
static RegFileTrace *regFileTrace;
static MemTrace     *memTrace;

static struct dbg_state dbg_state;

static map<address, bool> breakpoints;

void dbg_sys_init(TcpServer &tS, CpuTrace &cT, RegFileTrace &rT, MemTrace &mT)
{
    tcpServer       = &tS;
    cpuTrace        = &cT;
    regFileTrace    = &rT;
    memTrace        = &mT;

    cpuTrace->pcTraceIt = cpuTrace->pcTrace.begin();

    for(int i=0;i<32;++i){
        dbg_state.registers[i] = 0xdeadbeef;
    }

    dbg_sys_update_state();

    int ret;
    do{
        ret = dbg_main(&dbg_state);
    } while(ret == 0);

    LOG_ERROR("GDB client disconnected.");
}

void dbg_sys_update_state()
{
    for(int i=0;i<32;++i){
        uint64_t value;
        if (regFileTrace->getValue(cpuTrace->pcTraceIt->time, i, &value)){
            dbg_state.registers[i] = (uint32_t)value;
        }
    }

    dbg_state.registers[DBG_CPU_RISCV_PC] = cpuTrace->pcTraceIt->pc;
}


#define RXBUF_SIZE      256
static unsigned char rxbuf[RXBUF_SIZE];
static int  rxbuf_cur_idx = 0;
static int  rxbuf_len = 0;

int dbg_sys_getc(void)
{
    if (rxbuf_cur_idx != rxbuf_len){
        return rxbuf[rxbuf_cur_idx++];
    }

    ssize_t ret = tcpServer->recv((void *)rxbuf, RXBUF_SIZE);
    if (ret > 0){
        rxbuf_cur_idx   = 0;
        rxbuf_len       = ret;
        return rxbuf[rxbuf_cur_idx++];
    }
    else{
        return EOF;
    }
}

#define TXBUF_SIZE      256
static unsigned char txbuf[TXBUF_SIZE];

int dbg_sys_putchar(int ch)
{
    txbuf[0] = ch;
    size_t ret = tcpServer->xmit((void *)txbuf, 1);
    if (ret > 0){
        return ch;
    }
    else{
        return EOF;
    }
}

int dbg_sys_mem_readb(address addr, char *val)
{
    memTrace->getValue(cpuTrace->pcTraceIt->time, addr, val);
    return 0;
}

int dbg_sys_mem_writeb(address addr, char val)
{
    return 0;
}

int dbg_sys_continue(void)
{
    while(cpuTrace->pcTraceIt != cpuTrace->pcTrace.end()){
        address curAddr = cpuTrace->pcTraceIt->pc;

        LOG_INFO("PC: 0x%08x", curAddr);

        auto breakpointIt = breakpoints.find(curAddr);
        if (breakpointIt != breakpoints.end()){
            LOG_INFO("dbg_sys_continue: hit breakpoint at PC = 0x%08lx (time: %ld, %ld out of %ld)", cpuTrace->pcTraceIt->pc, cpuTrace->pcTraceIt->time, std::distance(cpuTrace->pcTrace.begin(), cpuTrace->pcTraceIt), cpuTrace->pcTrace.size());
            break;
        }

        ++cpuTrace->pcTraceIt;
    }

    if (cpuTrace->pcTraceIt == cpuTrace->pcTrace.end()){
        LOG_INFO("Reached end of execution!!!");
        cpuTrace->pcTraceIt = cpuTrace->pcTrace.end() -1;
    }

    dbg_state.signum    = 0x05;         // SIGTRAP
    dbg_sys_update_state();

    return 0;
}

int dbg_sys_step(void)
{
    if (cpuTrace->pcTraceIt != cpuTrace->pcTrace.end()){
        ++cpuTrace->pcTraceIt;
    }

    if (cpuTrace->pcTraceIt == cpuTrace->pcTrace.end()){
        LOG_INFO("Reached end of execution!!!");
        cpuTrace->pcTraceIt = cpuTrace->pcTrace.end()-1;

        // -1 will ultimately result in a terminate message.
        return -1;
    }
    else{
        dbg_state.signum    = 0x05;         // SIGTRAP
    }

    LOG_INFO("dbg_sys_step: PC = 0x%08lx (time: %ld, %ld out of %ld)", cpuTrace->pcTraceIt->pc, cpuTrace->pcTraceIt->time, std::distance(cpuTrace->pcTrace.begin(), cpuTrace->pcTraceIt), cpuTrace->pcTrace.size());

    dbg_sys_update_state();

    return 0;
}

int dbg_sys_restart(void)
{
    cpuTrace->pcTraceIt = cpuTrace->pcTrace.begin();
    return 0;
}



int dbg_sys_add_breakpoint(address addr)
{
    auto breakpointIt = breakpoints.find(addr);

    if (breakpointIt == breakpoints.end()){
        breakpoints[addr] = true;
        LOG_INFO(">>>>>>>>> Breakpoint added: 0x%08x. Nr of breakpoints: %ld", addr, breakpoints.size());
    }
    
    return 0;
}

int dbg_sys_delete_breakpoint(address addr)
{
    auto breakpointIt = breakpoints.find(addr);

    if (breakpointIt != breakpoints.end()){
        breakpoints.erase(breakpointIt);
        LOG_INFO("<<<<<<<<< Breakpoint deleted: 0x%08x. Nr of breakpoints: %ld", addr, breakpoints.size());
    }

    return 0;
}

