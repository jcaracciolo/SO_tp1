#NamedPipes or Sockets
com=NamedPipes

ifeq ($(com),Sockets)
COMPath=../Server/Coms/Sockets
else ifeq ($(com),NamedPipes)
COMPath=../Server/Coms/NamedPipes
endif

COMS= $(wildcard $(COMPath)/*.c) 
COMSh= $(wildcard $(COMPath)/*.h)

OBJ= $(subst $(COMPath),.,$(COMS:.c=.o))
OBJ+= client.o

.PHONY: clean all

all:
	gcc -c $(COMS) $(COMSh) client.c
	gcc -o ./client $(OBJ)
	rm *.o

clean:
	rm -f client
