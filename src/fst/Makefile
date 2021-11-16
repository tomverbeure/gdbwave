
CFLAGS      += -I. -g
OBJ_FILES   = fastlz.o lz4.o fstapi.o
AR_FILE     = libfstapi.a

$(AR_FILE): $(OBJ_FILES)
	ar rcs $(AR_FILE) $(OBJ_FILES)

clean:
	rm -fr *.o *.a
