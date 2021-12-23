#ifndef RISCV_H
#define RISCV_H

//exceptions
#define CAUSE_ILLEGAL_INSTRUCTION 2
#define CAUSE_MACHINE_TIMER 7
#define CAUSE_SCALL 9

//interrupts
#define CAUSE_MACHINE_EXTERNAL 11


#define MEDELEG_INSTRUCTION_PAGE_FAULT (1 << 12)
#define MEDELEG_LOAD_PAGE_FAULT (1 << 13)
#define MEDELEG_STORE_PAGE_FAULT (1 << 15)
#define MEDELEG_USER_ENVIRONNEMENT_CALL (1 << 8)
#define MIDELEG_SUPERVISOR_SOFTWARE (1 << 1)
#define MIDELEG_SUPERVISOR_TIMER (1 << 5)
#define MIDELEG_SUPERVISOR_EXTERNAL (1 << 9)

#define MIP_STIP (1 << 5)
#define MIE_MTIE (1 << CAUSE_MACHINE_TIMER)
#define MIE_MEIE (1UL << CAUSE_MACHINE_EXTERNAL)

#define MSTATUS_UIE         0x00000001UL
#define MSTATUS_SIE         0x00000002UL
#define MSTATUS_HIE         0x00000004UL
#define MSTATUS_MIE         0x00000008UL
#define MSTATUS_UPIE        0x00000010UL
#define MSTATUS_SPIE        0x00000020UL
#define MSTATUS_HPIE        0x00000040UL
#define MSTATUS_MPIE        0x00000080UL
#define MSTATUS_SPP         0x00000100UL
#define MSTATUS_HPP         0x00000600UL
#define MSTATUS_MPP         0x00001800UL
#define MSTATUS_FS          0x00006000UL
#define MSTATUS_XS          0x00018000UL
#define MSTATUS_MPRV        0x00020000UL
#define MSTATUS_SUM         0x00040000UL
#define MSTATUS_MXR         0x00080000UL
#define MSTATUS_TVM         0x00100000UL
#define MSTATUS_TW          0x00200000UL
#define MSTATUS_TSR         0x00400000UL
#define MSTATUS32_SD        0x80000000UL
#define MSTATUS_UXL         0x0000000300000000U
#define MSTATUS_SXL         0x0000000C00000000U
#define MSTATUS64_SD        0x8000000000000000U

#define SSTATUS_UIE         0x00000001UL
#define SSTATUS_SIE         0x00000002UL
#define SSTATUS_UPIE        0x00000010UL
#define SSTATUS_SPIE        0x00000020UL
#define SSTATUS_SPP         0x00000100UL
#define SSTATUS_FS          0x00006000UL
#define SSTATUS_XS          0x00018000UL
#define SSTATUS_SUM         0x00040000UL
#define SSTATUS_MXR         0x00080000UL
#define SSTATUS32_SD        0x80000000UL
#define SSTATUS_UXL         0x0000000300000000U
#define SSTATUS64_SD        0x8000000000000000U


#define PMP_R       0x01
#define PMP_W       0x02
#define PMP_X       0x04
#define PMP_A       0x18
#define PMP_L       0x80
#define PMP_SHIFT   2

#define PMP_TOR     0x08
#define PMP_NA4     0x10
#define PMP_NAPOT   0x18

#define MSCRATCH    0x340
#define MCAUSE      0x342
#define MEPC        0x341
#define MEPC        0x341
#define MTVAL       0x343

#define RDCYCLE     0xC00
#define RDTIME      0xC01
#define RDINSTRET   0xC02
#define RDCYCLEH    0xC80
#define RDTIMEH     0xC81
#define RDINSTRETH  0xC82


#define csr_swap(csr, val)					\
__extension__ ({						\
	uint32_t __v = (uint32_t)(val);				\
	__asm__ __volatile__ ("csrrw %0, " #csr ", %1"		\
			      : "=r" (__v) : "rK" (__v));	\
	__v;							\
})

#define csr_read(csr)						\
__extension__ ({						\
	uint32_t __v;						\
	__asm__ __volatile__ ("csrr %0, " #csr			\
			      : "=r" (__v));			\
	__v;							\
})

#define csr_write(csr, val)					\
{								\
	uint32_t __v = (uint32_t)(val);				\
	__asm__ __volatile__ ("csrw " #csr ", %0"		\
			      : : "rK" (__v));			\
}

#define csr_read_set(csr, val)					\
__extension__ ({						\
	uint32_t __v = (uint32_t)(val);				\
	__asm__ __volatile__ ("csrrs %0, " #csr ", %1"		\
			      : "=r" (__v) : "rK" (__v));	\
	__v;							\
})

#define csr_set(csr, val)					\
{								\
	uint32_t __v = (uint32_t)(val);				\
	__asm__ __volatile__ ("csrs " #csr ", %0"		\
			      : : "rK" (__v));			\
}

#define csr_read_clear(csr, val)				\
__extension__ ({						\
	uint32_t __v = (uint32_t)(val);				\
	__asm__ __volatile__ ("csrrc %0, " #csr ", %1"		\
			      : "=r" (__v) : "rK" (__v));	\
	__v;							\
})

#define csr_clear(csr, val)					\
{								\
	uint32_t __v = (uint32_t)(val);				\
	__asm__ __volatile__ ("csrc " #csr ", %0"		\
			      : : "rK" (__v));			\
}



#endif


