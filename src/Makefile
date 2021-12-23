

INC_FILES   = FstProcess.h CpuTrace.h
OBJ_FILES   = main.o FstProcess.o CpuTrace.o TcpServer.o gdbstub.o gdbstub_sys.o
LIB_FILES   = -lfstapi -lz

RISCV_TOOLCHAIN = /opt/riscv64-unknown-elf-toolchain-10.2.0-2020.12.8-x86_64-linux-ubuntu14/bin/
RISCV_PREFIX    = riscv64-unknown-elf-

GDB             = $(RISCV_TOOLCHAIN)/$(RISCV_PREFIX)gdb

CXXFLAGS    += --std=c++14 -I. -Wall -pedantic -g -O0
LDFLAGS     += -L./fst -Wall -g 

TEST_FST        = ../test_data/top.fst
TEST_CLK        = TOP.top.u_vex.cpu.clk
TEST_PC         = TOP.top.u_vex.cpu.lastStagePc 
TEST_PC_VALID   = TOP.top.u_vex.cpu.lastStageIsValid

all: gdbwave

run: gdbwave
	./gdbwave -w $(TEST_FST) -c $(TEST_CLK) -p $(TEST_PC) -e $(TEST_PC_VALID)

gdbwave: fst/libfstapi.a $(OBJ_FILES) $(INC_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS)  -o $@ $(OBJ_FILES) $(LIB_FILES)

fst/libfstapi.a:
	cd fst && make

$(OBJ_FILES): $(INC_FILES)

gdb: 
	$(GDB) ../test_data/progmem.elf -x ./gdb_conf.cmd

clean:
	\rm -fr gdbwave *.o
	cd fst && make clean
