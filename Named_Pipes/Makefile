(SRC)=$(wildcard *.c)
all: $(SRC)

	gcc -c coms.c
	gcc -c spawner.c
	gcc coms.o spawner.o -o run

	gcc -c client.c
	gcc client.o  coms.o -o client

	make clean

clean:
	rm *.o
	rm -f /tmp/*fifo*
