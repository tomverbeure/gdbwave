
#include <cstdio>

#include "TcpServer.h"

#include "gdbstub.h"
#include "gdbstub_sys.h"

static TcpServer    *tcpServer;
static CpuTrace     *cpuTrace;

static struct dbg_state dbg_state;

void dbg_sys_init(TcpServer &tS, CpuTrace &cT)
{
    tcpServer   = &tS;
    cpuTrace    = &cT;

    cpuTrace->pcTraceIt = cpuTrace->pcTrace.begin();

    dbg_sys_update_state();

    while(1){
        dbg_main(&dbg_state);
    }
}

void dbg_sys_update_state()
{
    for(int i=0;i<32;++i){
        dbg_state.registers[i] = 0x0;
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
    return 0;
}

int dbg_sys_mem_writeb(address addr, char val)
{
    return 0;
}

int dbg_sys_continue(void)
{
    dbg_state.signum    = 0x02;         // HALTREQ - SIGINT
    dbg_sys_update_state();

    return 0;
}

int dbg_sys_step(void)
{
    if (cpuTrace->pcTraceIt != cpuTrace->pcTrace.end()){
        ++cpuTrace->pcTraceIt;
    }

    printf("dbg_sys_step: PC = 0x%08lx\n", cpuTrace->pcTraceIt->pc);

    dbg_state.signum    = 0x05;         // STEP - SIGTRAP
    dbg_sys_update_state();

    return 0;
}

int dbg_sys_restart(void)
{
    cpuTrace->pcTraceIt = cpuTrace->pcTrace.begin();
    return 0;
}


