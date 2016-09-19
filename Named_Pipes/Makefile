(SRC)=$(wildcard *.c)
all: $(SRC)

	gcc -c reader.c
	gcc reader.c -o reader

	gcc -c writer.c
	gcc writer.c -o writer

	gcc -c coms.c
	gcc -c spawner.c
	gcc coms.o spawner.o -o run

	make clean

clean:
	rm *.o
	rm /tmp/*fifo*
