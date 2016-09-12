SRCC=$(wildcard *.c)
SRCPP=$(wildcard *.cpp)
OBJ = $(SRCC:.c=.o) $(SRCPP:.cpp=.o)

.PHONY: clean all

all:
	echo a >> dummy
	rm $(wildcard *.o) $(wildcard *.gch) $(wildcard./run) dummy
	gcc -c $(SRCC) $(wildcard *.h)
	g++ -c $(SRCPP)
	g++ -o run $(OBJ)

