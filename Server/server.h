#ifndef SERVER_H
#define SERVER_H
#include "Coms/coms.h"
#include "DB/SQlite/SQLparser.h"
#include "DB/UUID_DataBase/data_types.h"

typedef struct{
    int n;
    connection* con;
}threadData;

typedef struct{
  char prodName[MAX_PROD_NAME_LENGHT+1];
  int initialPrice;
  int initialStock;
  int priceAt0Stock;
  double m;
}productPriceData_t;

typedef struct{
  int size;
  productPriceData_t * prods;
}dataPrices_t;



void createChild(connection * con);
void assist(connection* con);
void initializeUUID(unsigned int n);


void* getNUUID(UUIDArray* tofill);
void* readNUUID(threadData* t);


int connectDB(dbdata_t* DBdata);
void msglog(int priority,char* message);

#endif
