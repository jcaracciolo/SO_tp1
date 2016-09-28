#ifndef SQL_PARSER_H
#define SQL_PARSER_H

#include <semaphore.h>

typedef struct{
    int fdin;
    int fdout;
    sem_t* sem;
} dbdata_t;

int createTable(dbdata_t * dbData);

int insertIntoTable(dbdata_t * dbData, char * name, int stock, int price);

int getPrice(dbdata_t * dbData, char * prodName);

int getStock(dbdata_t * dbData, char * prodName);

int changeValue(dbdata_t * dbData, char * prodName, int stock, int price);

int exitDB(dbdata_t * dbData);


#endif