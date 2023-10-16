all: ian-proj1
	
ian-proj1: ian-proj1.o
	gcc ian-proj1.c -o ian-proj1 -lelf

ian-proj1.o: ian-proj1.c
	gcc ian-proj1.c -c -o ian-proj1.o

.PHONY: all

clean:
	rm -rf ian-proj1.o ian-proj1
