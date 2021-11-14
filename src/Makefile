

INC_FILES   = FstProcess.h CpuTrace.h
OBJ_FILES   = main.o FstProcess.o CpuTrace.o
LIB_FILES   = -lfstapi -lz

CXXFLAGS    += --std=c++14 -I. -L./fst -Wall -g -O0

all: gdbwave
	./gdbwave 

run: gdbwave
	./gdbwave

gdbwave: fst/libfstapi.a $(OBJ_FILES) $(INC_FILES)
	$(CXX) $(CXXFLAGS)  -o $@ $(OBJ_FILES) $(LIB_FILES)

fst/libfstapi.a:
	cd fst && make

$(OBJ_FILES): $(INC_FILES)

clean:
	\rm -fr gdbwave *.o
