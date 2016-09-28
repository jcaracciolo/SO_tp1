#NamedPipes or Sockets
com=NamedPipes
MARSHPath=../Server/Marshalling

ifeq ($(com),Sockets)
COMPath=../Server/Coms/Sockets
else ifeq ($(com),NamedPipes)
COMPath=../Server/Coms/NamedPipes
endif

COMS= $(wildcard $(COMPath)/*.c)
COMSh= $(wildcard $(COMPath)/*.h)

MARSH= $(wildcard $(MARSHPath)/*.c)
MARSHh= $(wildcard $(MARSHPath)/*.h)

OBJ= $(subst $(COMPath),.,$(COMS:.c=.o))
OBJ+= $(subst $(MARSHPath),.,$(MARSH:.c=.o))
OBJ+= client.o

.PHONY: clean all

all:
	gcc -c $(COMS)  $(MARSH) $(COMSh) client.c
	gcc -o ./client $(OBJ)
	rm *.o

clean:
	rm -f client
