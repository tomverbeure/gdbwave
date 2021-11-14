

INC_FILES   = FstProcess.h
OBJ_FILES   = main.o FstProcess.o
LIB_FILES   = -lfstapi -lz

CXXFLAGS    += --std=c++14 -I. -L./fst

all: gdbwave
	./gdbwave | head -30

run: gdbwave
	./gdbwave

gdbwave: fst/libfstapi.a $(OBJ_FILES) $(INC_FILES)
	$(CXX) $(CXXFLAGS)  -o $@ $(OBJ_FILES) $(LIB_FILES)

fst/libfstapi.a:
	cd fst && make

$(OBJ_FILES): $(INC_FILES)

clean:
	\rm -fr gdbwave *.o
