#ifndef SEMIHOSTING_H
#define SEMIHOSTING_H

//int32_t trace_write(const char* buf, uint32_t nbyte);

void sh_write0(const char* buf);
void sh_writec(char c);
char sh_readc(void);

int getchar(void);

#endif
