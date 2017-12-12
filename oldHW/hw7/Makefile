# First target is default target, if you just type:  make

FILE=cacheSim.c

default: run

run: simulator
	./cacheSim.out

gdb: simulator 
	gdb --args cacheSim.out

simulator: ${FILE}
	gcc -g -lm -o cacheSim.out ${FILE}

analyze: simulator
	clang-check --analyze cacheSim.c -- -g

emacs: ${FILE}
	emacs ${FILE}
vim: ${FILE}
	vim ${FILE}

clean:
	rm -f cacheSim.out a.out *~

# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.
# The shell will view $dir as a shell variable and expand it.
dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz
