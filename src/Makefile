
OBJ_FILES   = main.o
LIB_FILES   = ./fst/fstapi.a

CXXFLAGS      += -I.
CXXFLAGS      += -I./fst

gdbwave: $(OBJ_FILES) $(LIB_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	\rm -fr gdbwave *.o
