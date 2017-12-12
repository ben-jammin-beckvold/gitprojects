# First target is default target, if you just type:  make

FILENAME=load_test
SOURCE_EXT=.c
SOURCEFILE=${FILENAME}${SOURCE_EXT}
OUTFILE=${FILENAME}.o

INCLUDE_FLAGS=-I/home/benb/libusb/libusb
LIBRARIES=-lm -fopenmp

default: run

test: 
	gcc -g -o ${TESTOUT} ${TESTSOURCE} ${INCLUDE_FLAGS} ${LIBRARIES}
	sudo ./${TESTOUT}

run: simulator
	sudo ./${OUTFILE}

gdb: simulator 
	gdb --args ${OUTFILE}

simulator: ${SOURCEFILE}
	gcc -g -o ${OUTFILE} ${SOURCEFILE} ${DEFINES} ${INCLUDE_FLAGS} ${LIBRARIES}

analyze: simulator
	clang-check --analyze ${SOURCEFILE} -- -g

emacs: ${SOURCEFILE}
	emacs ${SOURCEFILE}
vim: ${FILE}
	vim ${SOURCEFILE}

clean:
	rm -f *.out *.o *~

# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.
# The shell will view $dir as a shell variable and expand it.
dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz
