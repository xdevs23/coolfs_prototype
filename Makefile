
CFS_CFLAGS := -DDEBUG

cfscli: libcfs.so *.h cli.cpp
	g++ $(CFS_CFLAGS) -L$(shell pwd) -o cfscli cli.cpp -lcfs

LIBCFS_SRC_FILES := \
	fs.cpp \
	inode.cpp \
	superblock.cpp

libcfs.so: $(LIBCFS_SRC_FILES) *.h
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
