#ifndef DATA_TYPES_H
#define DATA_TYPES_H
#include <stdint.h>

#define STOCK_BLOCK_SIZE 11
#define MAX_UUIDS_PER_ARRAY 10
#define MAX_PROD_NAME_LENGHT 10

typedef enum  {CLOSE,PRICE, STOCK, BUY, SELL, EXISTS} transType_t;

typedef struct{
    transType_t transType;
    char prodName[MAX_PROD_NAME_LENGHT+1];  //+1 because of \0

} query_t;

typedef struct{
    uint64_t  high;
    uint64_t  low;
} UUID;

typedef struct{
    UUID* uuids;
    int size;
    int last;
} UUIDStock;

typedef struct{
    UUID uuids[MAX_UUIDS_PER_ARRAY];
    int size;
} UUIDArray;



#endif
