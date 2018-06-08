#   :set noexpandtab
# makefile needs tabs not spaces before command lines

CC=gcc

all: sysfs_blink kernel_blink

sysfs_blink:
	$(CC) -o sysfs_blink.o sysfs_blink.c

kernel_blink:
	$(CC) -o sysfs_blink.o kernel_blink.c

.PHONY: clean

clean:
	rm -f *.o
 
