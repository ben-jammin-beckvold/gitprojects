#CROSS_COMPILE := arm-linux-gnueabi-

CC	:= $(CROSS_COMPILE)gcc

DEFINE='NUMRANGERS=1'

CFLAGS	:= -Wall -std=gnu11 -ggdb -D$(DEFINE)

SRC	:= vl53Powerup.c
OBJ	:= $(SRC:.c=.o)

PROG	:= vl53Powerup

all: $(PROG)

run: $(PROG)
	./$(PROG)

$(PROG): $(OBJ)
	$(CC) -static $^ -o $@

clean:
	$(RM) $(PROG) $(OBJ)
