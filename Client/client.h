#ifndef CLIENT_H
#define CLIENT_H

#include "../Server/DB/UUID_DataBase/data_types.h"
#define MAX_PRODUCTS 2

char validProd[MAX_PRODUCTS][MAX_PROD_NAME_LENGHT+1] = {"papa\0", "tomate\0"};

typedef struct{
    char prodName[MAX_PROD_NAME_LENGHT+1];
    int newPrice;
    int prevPrice;
    int remoteStock;
    int priceTrend;
    UUIDStock * stock;
} productInfo_t;



#endif
