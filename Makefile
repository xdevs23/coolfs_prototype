
CFS_CFLAGS := -DDEBUG

ifeq ($(shell echo -n I | od -to2 | awk '{ print substr($$2,6,1); exit}'),0)
$(info Little endian detected)
CFS_CFLAGS += -DLITTLE_E
else
$(info Big endian detected)
CFS_CFLAGS += -DBIG_E
endif

cfscli: libcfs.so *.h cli.cpp Makefile
	g++ $(CFS_CFLAGS) -L$(shell pwd) -o cfscli cli.cpp -lcfs

LIBCFS_SRC_FILES := \
	fs.cpp \
	inode.cpp \
	superblock.cpp

libcfs.so: $(LIBCFS_SRC_FILES) *.h Makefile
	g++ -shared -fPIC $(CFS_CFLAGS) -o libcfs.so $(LIBCFS_SRC_FILES)

.PHONY: run
run: cfscli
	LD_LIBRARY_PATH="$(shell pwd)" ./cfscli

.PHONY: clean
clean:
	rm -f ./cfscli
	rm -f ./libcfs.so
	rm -f ./*.o

.PHONY: all
all: cfscli libcfs.so
