(SRC)=$(wildcard *.c)
all: $(SRC)

	gcc -c coms.c
	gcc -c server.c
	gcc coms.o server.o -o ../server

	gcc -c client.c
	gcc client.o  coms.o -o ../client

	make clean

clean:
	rm *.o
	rm -f /tmp/*fifo*
