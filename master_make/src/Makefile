EXECUTABLE=../hellomake

IDIR=../include
ODIR=obj
LDIR =../lib

CC=gcc
CFLAGS=-I$(IDIR)

LIBS=-lm

_DEPS = hellomake.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS)) #adds the directory structure to each filename

_OBJ = hellomake.o hellofunc.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ)) #adds the directory structure to each filename

#.SILENT:	# silences echoing all recipes

$(ODIR)/%.o: %.c $(DEPS)
	echo $(OBJ)
	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: $(OBJ)
	$(CC) -o ../$@ $^ $(CFLAGS) $(LIBS)

vim:	# quick edit for makefile 
	vim Makefile

edit:	# edit project in Atom
	atom ../

# example for importing command line arguments
# make ARGS="something" run
# run: 
# 	$(EXECUTABLE) ${ARGS}
run: hellomake
	$(EXECUTABLE)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(EXECUTABLE) $(IDIR)/*~

# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.
# The shell will view $dir as a shell variable and expand it.
dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz
