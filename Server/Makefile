UUIDPath=./DB/UUID_DataBase
DBPath=./DB/SQlite
MARSHPath=./Marshalling

#NamedPipes or Sockets
com=NamedPipes

ifeq ($(com),Sockets)
COMPath=./Coms/Sockets
else ifeq ($(com),NamedPipes)
COMPath=./Coms/NamedPipes
endif

UUIDDB= $(wildcard $(UUIDPath)/*.cpp)
UUIDDBh= $(wildcard $(UUIDPath)/*.h)

DB= $(wildcard $(DBPath)/*.c)
DBh=  $(wildcard $(DBPath)/*.h)

COMS= $(wildcard $(COMPath)/*.c)
COMSh= $(wildcard $(COMPath)/*.h)

MARSH= $(wildcard $(MARSHPath)/*.c)
MARSHh= $(wildcard $(MARSHPath)/*.h)

OBJ= $(subst $(DBPath),.,$(DB:.c=.o))
OBJ+= $(subst $(UUIDPath)/, ,$(UUIDDB:.cpp=.o))
OBJ+= $(subst $(COMPath)/, ,$(COMS:.c=.o))
OBJ+= $(subst $(MARSHPath)/, ,$(MARSH:.c=.o))
OBJ+= server.o

.PHONY: clean all

all:
	#cd Logs; make
	gcc -g -c $(DB) $(COMS) $(SERVER) $(DBh) $(MARSH) $(MARSHh) $(COMSh) $(SERVERh) $(UUIDDBh) server.c server.h
	g++ -g -c $(UUIDDB)
	g++ -g -o run $(OBJ) -pthread
	rm *.o $(UUIDPath)/*.gch

clean:
	rm -f run
	rm -f ./Logs/log-*
