EXECUTABLE=../mag_encoder_driver


LIB_PATH=-L../

IDIR=../include
ODIR=obj
LDIR =../lib

CC=g++
CFLAGS=-I$(IDIR) #-I$(SERIAL_PATH)

LIBS=-lm -lserial

_DEPS = 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS)) #adds the directory structure to each filename

_OBJ = mag_encoder_driver.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ)) #adds the directory structure to each filename

#.SILENT:	# silences echoing all recipes

$(ODIR)/%.o: %.cpp $(DEPS)
	echo $(OBJ)
	$(CC) -c -o $@ $< $(CFLAGS)

mag_encoder_driver: $(OBJ)
	$(CC) -o ../$@ $^ $(CFLAGS) $(LIB_PATH) $(LIBS)

vim:	# quick edit for makefile 
	vim Makefile

edit:	# edit project in sublime-text
	subl ../

# example for importing command line arguments
# make ARGS="something" run
# run: 
# 	$(EXECUTABLE) ${ARGS}
run: mag_encoder_driver
	chmod +x $(EXECUTABLE)
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
