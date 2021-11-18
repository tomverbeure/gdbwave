

INC_FILES   = FstProcess.h CpuTrace.h
OBJ_FILES   = main.o FstProcess.o CpuTrace.o TcpServer.o
LIB_FILES   = -lfstapi -lz

CXXFLAGS    += --std=c++14 -I. -Wall -g -O0
LDFLAGS     += -L./fst -Wall -g 

all: gdbwave
	./gdbwave 

run: gdbwave
	./gdbwave

gdbwave: fst/libfstapi.a $(OBJ_FILES) $(INC_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS)  -o $@ $(OBJ_FILES) $(LIB_FILES)

fst/libfstapi.a:
	cd fst && make

$(OBJ_FILES): $(INC_FILES)

clean:
	\rm -fr gdbwave *.o
	cd fst && make clean
