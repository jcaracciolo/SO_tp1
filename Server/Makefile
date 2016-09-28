UUIDPath=./DB/UUID_DataBase
DBPath=./DB/SQlite

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

OBJ= $(subst $(DBPath),.,$(DB:.c=.o))
OBJ+= $(subst $(UUIDPath),.,$(UUIDDB:.cpp=.o))
OBJ+= $(subst $(COMPath),.,$(COMS:.c=.o))
OBJ+= server.o

.PHONY: clean all

all:
	gcc -c $(DB) $(COMS) $(SERVER) $(DBh) $(COMSh) $(SERVERh) $(UUIDDBh) server.c server.h  
	g++ -c $(UUIDDB)
	g++ -o run $(OBJ)
	rm *.o $(UUIDPath)/*.gch

	

