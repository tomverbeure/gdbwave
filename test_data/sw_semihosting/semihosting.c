#include <stdint.h>
#include <stdarg.h>

#include "printf.h"
#include "semihosting.h"

#define SEMIHOSTING_SYS_OPEN            0x01
#define SEMIHOSTING_SYS_CLOSE           0x02
#define SEMIHOSTING_SYS_WRITEC          0x03
#define SEMIHOSTING_SYS_WRITE0          0x04
#define SEMIHOSTING_SYS_WRITE           0x05
#define SEMIHOSTING_SYS_READ            0x06
#define SEMIHOSTING_SYS_READC           0x07
#define SEMIHOSTING_SYS_ISERROR         0x08
#define SEMIHOSTING_SYS_ISTTY           0x09
#define SEMIHOSTING_SYS_SEEK            0x0A
#define SEMIHOSTING_SYS_FLEN            0x0C
#define SEMIHOSTING_SYS_TMPNAM          0x0D
#define SEMIHOSTING_SYS_REMOVE          0x0E
#define SEMIHOSTING_SYS_RENAME          0x0F
#define SEMIHOSTING_SYS_CLOCK           0x10
#define SEMIHOSTING_SYS_TIME            0x11
#define SEMIHOSTING_SYS_SYSTEM          0x12
#define SEMIHOSTING_SYS_ERRNO           0x13
#define SEMIHOSTING_SYS_GET_CMDLINE     0x15
#define SEMIHOSTING_SYS_HEAPINFO        0x16
#define SEMIHOSTING_EnterSVC            0x17
#define SEMIHOSTING_ReportException     0x18
#define SEMIHOSTING_SYS_ELAPSED         0x30
#define SEMIHOSTING_SYS_TICKFREQ        0x31

#define OS_INTEGER_TRACE_TMP_ARRAY_SIZE 128

#define RISCV_SEMIHOSTING_CALL_NUMBER 7


static inline int __attribute__ ((always_inline)) call_host(int reason, void* arg) {
#if 1
    register int value asm ("a0") = reason;
    register void* ptr asm ("a1") = arg;
    asm volatile (
        // Workaround for RISC-V lack of multiple EBREAKs.
        " .option push \n"
        " .option norvc \n"
        // Force 16-byte alignment to make sure that the 3 instruction fall
        // within the same virtual page. If you the instruction straddle a page boundary
        // the debugger fetching the instructions could lead to a page fault.
        " .align 4 \n"
        " slli x0, x0, 0x1f \n"
        " ebreak \n"
        " srai x0, x0, %[swi] \n"
        " .option pop \n"

        : "=r" (value) /* Outputs */
        : "0" (value), "r" (ptr), [swi] "i" (RISCV_SEMIHOSTING_CALL_NUMBER) /* Inputs */
        : "memory" /* Clobbers */
    );
    return value;
#else
    return 0;    
#endif    
}

void sh_write0(const char* buf)
{
    // send string
    call_host(SEMIHOSTING_SYS_WRITE0, (void*) buf);
}

void sh_writec(char c)
{
    // send string
    call_host(SEMIHOSTING_SYS_WRITEC, (void*)&c);
}

char sh_readc(void)
{
    return call_host(SEMIHOSTING_SYS_READC, (void*)NULL);
}

void _putchar(char character)
{
    sh_writec(character);
}

int printf_(const char* format, ...)
{
    char buffer[128];

    va_list va;
    va_start(va, format);
    const int ret = vsnprintf(buffer, sizeof(buffer)-1, format, va);
    va_end(va);
    
    sh_write0(buffer);
    return ret;
}

int putchar(int c)
{
    sh_writec((char)c);
    return c;
}

int getchar(void)
{
    return (int)sh_readc();
}

#if 0
int fopen(const char *pathname, const char *mode)
{
}
#endif




