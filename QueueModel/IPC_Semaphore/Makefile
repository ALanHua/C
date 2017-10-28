#### PROJECT SETTINGS ####

SRCDIR 		:= .
INCDIR		:= -I./
CFLAGS 		:= -o2 -Wall
CFLAGS 		+=
LDFLAGS		:=


#SRC		:= $(wildcard SRCDIR/*.c)
SRC     := curr_time.c get_num.c
OBJS 	:= $(SRC:%.c=%.o)

all:$(OBJS)
	gcc $(INCDIR) $(CFLAGS) -o main main.c $^
.c.o:

	gcc -c $(CFLAGS) $(INCDIR) -o $@ $<

clean:
	rm -rf *.o semaphore

.PHONY : clean
