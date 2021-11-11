
CFLAGS      += -I.
OBJ_FILES   = fstapi.o fastlz.o lz4.o
AR_FILE     = fstapi.a

fstapi.a: $(OBJ_FILES)
	ar q $(AR_FILE) $(OBJ_FILES)


clean:
	rm -fr *.o *.a
