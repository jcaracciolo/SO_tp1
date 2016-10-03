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

.PHONY: clean all

all:
	gcc -g -c $(COMS)  $(MARSH) $(COMSh) client.c
	gcc -g $(OBJ) client.o -o client -lm
	rm *.o

	gcc -c $(COMS)  $(MARSH) $(COMSh) user.c
	gcc $(OBJ) user.o -o user
	rm *.o

clean:
	rm -f client
	rm -f user
