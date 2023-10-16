OBJS	= ipkcpc.o
SOURCE	= ipkcpc.c
HEADER	= ipkcpc.h
OUT	= ipkcpc
CC	 = gcc
FLAGS	 = -g -c -Wall -Werror
ifeq ($(OS),Windows_NT)
LFLAGS	 = -lws2_32
else
LFLAGS	 = 
endif

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

ipkcpc.o: ipkcpc.c
	$(CC) $(FLAGS) ipkcpc.c -std=c11


clean:
ifeq ($(OS),Windows_NT)
	del -f $(OBJS) $(OUT).exe
else
	rm -f $(OBJS) $(OUT)
endif
