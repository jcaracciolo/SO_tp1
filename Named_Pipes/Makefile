(SRC)=$(wildcard *.c)
all: $(SRC)

	gcc -c reader.c
	gcc reader.c -o reader

	gcc -c writer.c
	gcc writer.c -o writer

	gcc spawner.c

clean:
	rm *.o
