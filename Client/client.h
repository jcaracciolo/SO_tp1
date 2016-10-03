#ifndef CLIENT_H
#define CLIENT_H

#include "../Server/DB/UUID_DataBase/data_types.h"

//If you want the client to trade another good, just increase MAX_PRODUCTS
//and add the product name to the validProd list.
#define MAX_PRODUCTS 4
char validProd[MAX_PRODUCTS][MAX_PROD_NAME_LENGHT+1] =
    {"papa\0", "tomate\0","pepino\0","cebolla\0"};

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



#endif
