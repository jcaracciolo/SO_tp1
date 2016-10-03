#ifndef CLIENT_H
#define CLIENT_H

#include "../Server/DB/UUID_DataBase/data_types.h"
#include "../Server/Coms/coms.h"
#include "../Server/Marshalling/marsh.h"
#include "../Server/DB/UUID_DataBase/data_types.h"
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>


//If you want the client to trade another good, just increase MAX_PRODUCTS
//and add the product name to the validProd list.
#define MAX_PRODUCTS 7
char validProd[MAX_PRODUCTS][MAX_PROD_NAME_LENGHT+1] =
    {"papa", "tomate","pepino","cebolla","zucchini","zapallito","remolacha"};

//Positive price trend means the prise is rising
typedef struct{
    char prodName[MAX_PROD_NAME_LENGHT+1];
    int newPrice;
    int prevPrice;
    int remoteStock;
    int opsSincePrice;
    int opsSinceStock;
    int priceTrend;
    int productProfit;
    int investedInStock;
    UUIDStock * stock;
} productInfo_t;

char* conerrormsg[]={
        "Conection lost",
        "Insuficient amount of products",
        "Insuficient stock",
        "Maximun amount of UUIDs per transaction exceded",
        "Money provided not enough to concrete purchase",
        "Transaction revenue not enough to reach minimal payment",
        "Invalid UUIDs",
        "No such element available"};

void printError(conerrors_t error);


#endif
