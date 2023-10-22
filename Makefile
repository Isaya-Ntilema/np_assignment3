CC = gcc
CC_FLAGS = -w -g

all: test cchat cserverd

main_curses.o: main_curses.c
	$(CC) -Wall -I. -c main_curses.c

test: main_curses.o
	$(CC) -I./ -Wall main_curses.o -lncurses -o test -pthread

client.o: client.c
	$(CC) -c -Wall client.c -o client.o -pthread

cchat: client.o
	$(CC) -Wall -o cchat client.o -pthread

server.o: server.c
	$(CC) -c -Wall server.c -o server.o -pthread

cserverd: server.o
	$(CC) -Wall -o cserverd server.o -pthread

clean:
	rm -f *.o test cchat cserverd
