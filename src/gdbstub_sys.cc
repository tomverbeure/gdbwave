
#include <cstdio>

#include "TcpServer.h"

#include "gdbstub_sys.h"

static TcpServer    *tcpServer;
static CpuTrace     *cpuTrace;

void dbg_sys_init(TcpServer &tS, CpuTrace &cT)
{
    tcpServer   = &tS;
    cpuTrace    = &cT;

    cpuTrace->pcTraceIt = cpuTrace->pcTrace.begin();
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
    return 0;
}

int dbg_sys_step(void)
{
    if (cpuTrace->pcTraceIt != cpuTrace->pcTrace.end()){
        ++cpuTrace->pcTraceIt;
    }
    return 0;
}

