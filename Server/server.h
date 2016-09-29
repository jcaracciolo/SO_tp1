#ifndef SERVER_H
#define SERVER_H
#include "Coms/coms.h"
#include "DB/UUID_DataBase/data_types.h"


void createChild(connection * con);
void assist(connection* con);
void initializeUUID(unsigned int n);
int validateUUID(char* arg);
int getNUUID(UUIDArray* tofill);

#endif
