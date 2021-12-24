/*
 * Copyright (c) 2016-2019 Matt Borgerson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _GDBSTUB_SYS_H_
#define _GDBSTUB_SYS_H_

#include <cstddef>
#include <stdint.h>

#include "TcpServer.h"
#include "CpuTrace.h"

/*****************************************************************************
 * Types
 ****************************************************************************/

typedef unsigned int address;
typedef unsigned int reg;

#pragma pack(1)
struct dbg_interrupt_state {
	uint32_t ss;
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t vector;
	uint32_t error_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
};
#pragma pack()

#pragma pack(1)
struct dbg_idtr
{
	uint16_t len;
	uint32_t offset;
};
#pragma pack()

#pragma pack(1)
struct dbg_idt_gate
{
	uint16_t offset_low;
	uint16_t segment;
	uint16_t flags;
	uint16_t offset_high;
};
#pragma pack()

enum DBG_REGISTER {
	DBG_CPU_RISCV_REG_0         = 0,
	DBG_CPU_RISCV_REG_1         = 1,
	DBG_CPU_RISCV_REG_2         = 2,
	DBG_CPU_RISCV_REG_3         = 3,
	DBG_CPU_RISCV_REG_4         = 4,
	DBG_CPU_RISCV_REG_5         = 5,
	DBG_CPU_RISCV_REG_6         = 6,
	DBG_CPU_RISCV_REG_7         = 7,
	DBG_CPU_RISCV_REG_8         = 8,
	DBG_CPU_RISCV_REG_9         = 9,
	DBG_CPU_RISCV_REG_10        = 10,
	DBG_CPU_RISCV_REG_11        = 11,
	DBG_CPU_RISCV_REG_12        = 12,
	DBG_CPU_RISCV_REG_13        = 13,
	DBG_CPU_RISCV_REG_14        = 14,
	DBG_CPU_RISCV_REG_15        = 15,
	DBG_CPU_RISCV_REG_16        = 16,
	DBG_CPU_RISCV_REG_17        = 17,
	DBG_CPU_RISCV_REG_18        = 18,
	DBG_CPU_RISCV_REG_19        = 19,
	DBG_CPU_RISCV_REG_20        = 20,
	DBG_CPU_RISCV_REG_21        = 21,
	DBG_CPU_RISCV_REG_22        = 22,
	DBG_CPU_RISCV_REG_23        = 23,
	DBG_CPU_RISCV_REG_24        = 24,
	DBG_CPU_RISCV_REG_25        = 25,
	DBG_CPU_RISCV_REG_26        = 26,
	DBG_CPU_RISCV_REG_27        = 27,
	DBG_CPU_RISCV_REG_28        = 28,
	DBG_CPU_RISCV_REG_29        = 29,
	DBG_CPU_RISCV_REG_30        = 30,
	DBG_CPU_RISCV_REG_31        = 31,
	DBG_CPU_RISCV_PC            = 32,
	DBG_CPU_RISCV_NUM_REGISTERS = 33
};

struct dbg_state {
	int signum;
	reg registers[DBG_CPU_RISCV_NUM_REGISTERS];
};

/*****************************************************************************
 * Const Data
 ****************************************************************************/

extern void const * const dbg_int_handlers[];

/*****************************************************************************
 * Prototypes
 ****************************************************************************/

void dbg_sys_init(TcpServer &tS, CpuTrace &cT);
void dbg_sys_update_state();

int dbg_hook_idt(uint8_t vector, const void *function);
int dbg_init_gates(void);
int dbg_init_idt(void);
int dbg_load_idt(struct dbg_idtr *idtr);
int dbg_store_idt(struct dbg_idtr *idtr);
uint32_t dbg_get_cs(void);
void dbg_int_handler(struct dbg_interrupt_state *istate);
void dbg_interrupt(struct dbg_interrupt_state *istate);
void dbg_start(void);
void dbg_io_write_8(uint16_t port, uint8_t val);
uint8_t dbg_io_read_8(uint16_t port);
void *dbg_sys_memset(void *ptr, int data, size_t len);
int dbg_sys_restart(void);

#endif
