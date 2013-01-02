CC = gcc
LD = gcc

CFLAGS = -ansi -Wall -lncursesw -I include/ 
LDFLAGS = -ansi -Wall -lncursesw -I include/

SRCS = main.c gui.c init.c io.c login.c loop.c protocol.c structmem.c
OBJECTS = $(SRCS:.c=.o)
EXEC = client

OBJS = $(addprefix ./obj/, $(OBJECTS))

$(EXEC): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

./obj/%.o: ./src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)
