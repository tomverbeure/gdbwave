

OBJ_FILES   = main.o
LIB_FILES   = -lfstapi -lz

CXXFLAGS    += -I. -L./fst

gdbwave: fst/libfstapi.a $(OBJ_FILES)
	$(CXX) $(CXXFLAGS)  -o $@ $(OBJ_FILES) $(LIB_FILES)

fst/libfstapi.a:
	cd fst && make

clean:
	\rm -fr gdbwave *.o
