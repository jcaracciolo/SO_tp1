#ifndef CLIENT_H
#define CLIENT_H


#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include "../Server/Coms/coms.h"
#include "../Server/Marshalling/marsh.h"
#include "../Server/DB/UUID_DataBase/data_types.h"

#define END_OF_CONNECTION "KILLMEPLZ"
#define TIME_SCALE 3
#define OPS_PER_TICK 1
#define MAX_BUF 300
#define CONSERV_RELATION 3


//If you want the client to trade another good, just increase MAX_PRODUCTS
//and add the product name to the validProd list.
#define MAX_PRODUCTS 2
char validProd[MAX_PRODUCTS][MAX_PROD_NAME_LENGHT+1] = {"papa\0", "tomate\0"};

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

void printProduct(productInfo_t * product);

int totalStock(productInfo_t * products);

int updatePrice(productInfo_t * product, int price);
                transType_t decideAction(int priceWeight,int stockWeight, int buyWeight, int sellWeight);

int initProducts(productInfo_t * products);

int decideWhatToBuy(connection * con, productInfo_t * product, int cash,
                    int conservativeness,int pid);

int decideWhatToSell( connection * con, productInfo_t * product,
                      int conservativeness,int pid);

char *conerrors[5]={   "Insufficient stock for that product",
                       "Amount of units surpass maximum limit",
                       "Money provided not enough for the transaction",
                       "Products worth less than required",
                       "UUIDS invalid"};



#endif
