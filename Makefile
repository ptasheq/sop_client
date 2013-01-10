CC = gcc
LD = gcc

CFLAGS = -ansi -Wall -lncursesw -I include/ -g 
LDFLAGS = -ansi -Wall -lncursesw -I include/ -g

SRCS = communication.c main.c gui.c init.c io.c listener.c login.c loop.c protocol.c structmem.c
OBJECTS = $(SRCS:.c=.o)
EXEC = client

OBJS = $(addprefix ./obj/, $(OBJECTS))

$(EXEC): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

./obj/%.o: ./src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)
