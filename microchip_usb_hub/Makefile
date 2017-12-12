# First target is default target, if you just type:  make

FILENAME=hub_ctrl
SOURCE_EXT=.cpp
SOURCEFILE=${FILENAME}${SOURCE_EXT}
OUTFILE=${FILENAME}.o

TESTNAME=testlibusb
TESTSOURCE=${TESTNAME}${SOURCE_EXT}
TESTOUT=${TESTNAME}.o

INCLUDE_FLAGS=-I/home/benb/libusb/libusb -std=c++11
LIBRARIES=-lusb-1.0
#DEFINES=-DGPIO_TEST
DEFINES=-DI2C_TEST

default: run

test:
	g++ -g -o ${TESTOUT} ${TESTSOURCE} ${INCLUDE_FLAGS} ${LIBRARIES}
	sudo ./${TESTOUT}

run: build
	sudo ./${OUTFILE}

gdb: build
	gdb --args ${OUTFILE}

build: ${SOURCEFILE}
	gcc -g -o ${OUTFILE} ${SOURCEFILE} ${DEFINES} ${INCLUDE_FLAGS} ${LIBRARIES}

analyze: build
	clang-check --analyze ${SOURCEFILE} -- -g

emacs: ${SOURCEFILE}
	emacs ${SOURCEFILE}
cpp: ${FILE}
	vim ${SOURCEFILE}
head: ${FILE}
	vim ${FILENAME}.h

clean:
	rm -f *.out *.o *~

# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.
# The shell will view $dir as a shell variable and expand it.
dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz
