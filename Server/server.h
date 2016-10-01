#ifndef SERVER_H
#define SERVER_H
#include "Coms/coms.h"
#include "DB/SQlite/SQLparser.h"
#include "DB/UUID_DataBase/data_types.h"

typedef struct{
    int n;
    connection* con;
}threadData;

void createChild(connection * con);
void assist(connection* con);
void initializeUUID(unsigned int n);


void* getNUUID(UUIDArray* tofill);
void* readNUUID(threadData* t);


int connectDB(dbdata_t* DBdata);
void log(int priority,char* message);

#endif
