
all:
	cd Named_Pipes && \
	\
	gcc -c reader.c -o ../Build/reader.o && \
	\
	gcc -c writer.c -o ../Build/writer.o && \
	\
	gcc spawner.c

clean:
	cd Named_Pipes && \
	rm *.o
