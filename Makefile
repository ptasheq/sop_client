CC = gcc
LD = gcc

CFLAGS = -Wall -lncursesw -I include/ 
LDFLAGS = -Wall -lncursesw -I include/

SRCS = main.c gui.c init.c io.c login.c loop.c
OBJECTS = $(SRCS:.c=.o)
#HEADERS = gui.h libs.h login.h protocol.h
EXEC = client

OBJS = $(addprefix ./obj/, $(OBJECTS))
#HDRS = $(addprefix ./include/, $(HEADERS))

$(EXEC): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

./obj/%.o: ./src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)
