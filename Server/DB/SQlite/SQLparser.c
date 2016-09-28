#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "SQLparser.h"


int createTable(dbdata_t * dbData) {
	char str[200] = {0}, * query = "create table product (name  varchar(20) primary key, stock int, price int);\n";

    write(dbData->fdin, query, strlen(query));

    read(dbData->fdout, str, 200);
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }
    return 0;
}

int insertIntoTable(dbdata_t * dbData, char * name, int stock, int price) {
	char str[200] = {0};
	char * query = calloc(strlen(name) + 35 + 10,1 ); // query + name + stock + price '\n'	(numbers must have less than 5 digits)

	sprintf(query, "insert into product values('%s', %i, %i);\n", name, stock, price);

	write(dbData->fdin, query, strlen(query));

    read(dbData->fdout, str, 200);
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }

    free(query);
    return 0;
}

int exitDB(dbdata_t * dbData) {
	char str[200] = {0}, * query = ".exit\n";

    write(dbData->fdin, query, strlen(query));

    read(dbData->fdout, str, 200);
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }
    return 0;
}

int getPrice(dbdata_t * dbData, char * prodName) {
	char str[200] = {0};
	int nameLen = strlen(prodName);
	char * query = malloc(nameLen + 50); // query + name + '\n'

	sprintf(query, "select price from product where name = '%s';\n", prodName);

	write(dbData->fdin, query, strlen(query));

	read(dbData->fdout, str, 200);
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }

	char * ans = str + strlen(query);

	free(query);

	return atoi(ans);
	
}

int getStock(dbdata_t * dbData, char * prodName) {
	char str[200] = {0};
	int nameLen = strlen(prodName);
	char * query = calloc(nameLen + 50, 1); // query + name + '\n'

	sprintf(query, "select stock from product where name = '%s';\n", prodName);

	write(dbData->fdin, query, strlen(query));

	read(dbData->fdout, str, 200) > 0;
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }
	
	char * ans = str + strlen(query);

	free(query);

	return atoi(ans);
	
}

int changeValue(dbdata_t * dbData, char * prodName, int stock, int price) {	
	char str[200] = {0};
	int nameLen = strlen(prodName);
	char * query = calloc(nameLen + 55 + 10, 1); // query + name + stock + price '\n'	(numbers must have less than 5 digits)

	sprintf(query, "update product set stock = %i, price = %i where name = '%s';\n", stock, price, prodName);

	write(dbData->fdin, query, strlen(query));

	read(dbData->fdout, str, 200);
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }

	free(query);

	return 0;
}
