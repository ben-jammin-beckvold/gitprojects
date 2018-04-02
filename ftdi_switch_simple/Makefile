# First target is default target, if you just type:  make

FILENAME=usb_switch
SOURCE_EXT=.cpp
SOURCEFILE=${FILENAME}${SOURCE_EXT}
OUTFILE=${FILENAME}.o

pwd=${PWD}
LIB_DIR=-L${pwd}/libraries
DEFINES=#-DTOGGLE_TEST
INCLUDE_FLAGS=
LIBRARIES=-lftd2xx

default: run

run: build
	sudo ./${OUTFILE}
#	echo "finished"

gdb: build 
	gdb --args ${OUTFILE}

build: ${SOURCEFILE}
	g++ -g -Wall -o ${OUTFILE} ${SOURCEFILE} ${DEFINES} ${LIB_DIR} ${INCLUDE_FLAGS} ${LIBRARIES}

analyze: build
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
