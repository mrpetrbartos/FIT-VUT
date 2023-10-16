OBJS	= ipk-sniffer.o
OUT		= ipk-sniffer
CC	 	= g++
FLAGS	= -g -c -Wall -Wpedantic -Wextra
LFLAGS	= -lpcap

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

ipk-sniffer.o: ipk-sniffer.cpp
	$(CC) $(FLAGS) ipk-sniffer.cpp -std=c++20


clean:
	rm -f $(OBJS) $(OUT)
