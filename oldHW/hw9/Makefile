# First target is default target, if you just type:  make

FILE1=prod_cons_cond.c
FILE2=read_write.c

default: condition_var

prod_cons: condition_var
	./prod_cons.out

read_write: condition_var
	./read_write.out

condition_var: ${FILE}
	gcc -g -lpthread -o read_write.out ${FILE2}
	gcc -g -lpthread -o prod_cons.out ${FILE1}

clean:
	rm -f prod_cons.out read_write.out a.out *~

# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.
# The shell will view $dir as a shell variable and expand it.
dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz
