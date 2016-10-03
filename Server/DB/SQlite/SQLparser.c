#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "SQLparser.h"
#include "../../server.h"
#include "../../Logs/log.h"

#define MAX_BUFF 250


int createTable(dbdata_t * dbData) {
	char str[MAX_BUFF] = {0}, * query = "create table product (name  varchar(20) primary key, stock int, price int);\n";

    write(dbData->fdin, query, strlen(query));

    read(dbData->fdout, str, MAX_BUFF);
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }
    return 0;
}

int insertIntoTable(dbdata_t * dbData, char * name, int stock, int price) {
	char str[MAX_BUFF] = {0};
	char * query = calloc(strlen(name) + 35 + 10,1 ); // query + name + stock + price '\n'	(numbers must have less than 5 digits)

	sprintf(query, "insert into product values('%s', %i, %i);\n", name, stock, price);

	write(dbData->fdin, query, strlen(query));

    read(dbData->fdout, str, MAX_BUFF);
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }

    free(query);
    return 0;
}

int exitDB(dbdata_t * dbData) {
	char str[MAX_BUFF] = {0}, * query = ".exit\n";

    write(dbData->fdin, query, strlen(query));

    read(dbData->fdout, str, MAX_BUFF);
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }

    close(dbData->fdin);
    close(dbData->fdout);

    return 0;
}

int existsInDB(dbdata_t * dbData, char * prodName) {
    char str[MAX_BUFF] = {0};
    int nameLen = strlen(prodName);
    char * query = malloc(nameLen + 70); // query + name + '\n'

    sprintf(query, "select name from product where name = '%s';\n", prodName);

    write(dbData->fdin, query, strlen(query));

    read(dbData->fdout, str, MAX_BUFF);
    if (strncmp(str, query, strlen(query)) != 0) {
        return -1;
    }

    if (strcmp(str + strlen(query), "") == 0) {
        free(query);
        return 0;
    }
    free(query);
    return 1;
}

int getPrice(dbdata_t * dbData, char * prodName) {
	char str[MAX_BUFF] = {0};
    int nameLen = strlen(prodName);
    char * query = calloc(nameLen + 50,1); // query + name + '\n'

    sprintf(query, "select price from product where name = \'%s\';\n", prodName);

    write(dbData->fdin, query, strlen(query));

	read(dbData->fdout, str, MAX_BUFF);
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }

	char * ans = str + strlen(query);

	free(query);

	return atoi(ans);
	
}

int getStock(dbdata_t * dbData, char * prodName) {
	char str[MAX_BUFF] = {0};
	int nameLen = strlen(prodName);
	char * query = calloc(nameLen + 50, 1); // query + name + '\n'

	sprintf(query, "select stock from product where name = '%s';\n", prodName);

	write(dbData->fdin, query, strlen(query));

	read(dbData->fdout, str, MAX_BUFF) > 0;
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }

	char * ans = str + strlen(query);

    free(query);

	return atoi(ans);

}

int updatePrice(dbdata_t * dbData, char * prodName, int price) {
    char str[MAX_BUFF] = {0};
    int stock = getStock(dbData, prodName);
    int nameLen = strlen(prodName);
    char * query = calloc(nameLen + 57 + 20,1); // query + name + stock + price '\n'    (numbers must have less than 10 digits)

    sprintf(query, "update product set stock = %i, price = %i where name = '%s';\n", stock, price, prodName);

    write(dbData->fdin, query, strlen(query));

    read(dbData->fdout, str, MAX_BUFF);
    if (strncmp(str, query, strlen(query)) != 0) {
        return -1;
    }

    free(query);

    return 0;
}

int updateStock(dbdata_t * dbData, char * prodName, int stock) {
    char str[MAX_BUFF] = {0};
    int nameLen = strlen(prodName);
    char * query = calloc(nameLen + 57 + 20, 1); // query + name + stock + price '\n'    (numbers must have less than 10 digits)

    sprintf(query, "update product set stock = %i where name = '%s';\n", stock, prodName);

    write(dbData->fdin, query, strlen(query));

    read(dbData->fdout, str, MAX_BUFF);
    printf("%s",str);
    if (strncmp(str, query, strlen(query)) != 0) {
        return -1;
    }

    free(query);

    return 0;
}

int updateTable(dbdata_t * dbData, char * prodName, int stock, int price) {
	char str[MAX_BUFF] = {0};
	int nameLen = strlen(prodName);
	char * query = calloc(nameLen + 55 + 20, 1); // query + name + stock + price '\n'	(numbers must have less than 10 digits)

	sprintf(query, "update product set stock = %i, price = %i where name = '%s';\n", stock, price, prodName);

	write(dbData->fdin, query, strlen(query));

	read(dbData->fdout, str, MAX_BUFF);
    if (strncmp(str, query, strlen(query)) != 0) {
    	return -1;
    }

	free(query);

	return 0;
}

int checkDBConnection(dbdata_t* DBdata){
    char *msg = "create table hola(a int);insert into hola values(1);select * from hola;drop table hola;\n";
    char *msgerror = "TEST;\n";
    char str[MAX_BUFF] = {0};

    printf("Checking Database input...\n\n");
    log(INFO,"Checking Database input...");

    write(DBdata->fdin, msg, strlen(msg));

    printf("Checking Database output...\n\n");
    log(INFO,"Checking Database output...");
    read(DBdata->fdout, str, MAX_BUFF);

    if(strcmp(str,"create table hola(a int);\ninsert into hola values(1);\nselect * from hola;\n1\ndrop table hola;\n")!=0) {
        printf("ERROR CONNECTING DATABASE I/O\n");
        log(MERROR,"ERROR CONNECTING DATABASE I/O");
        printf("READ: %s", str);
        return -1;
    }

    printf("Database I/O connection successful\n\n");
    log(INFO,"Database I/O connection successful");

    memset(str, 0, 200);
    printf("Checking Database error output...\n\n");
    log(INFO,"Checking Database error output...");

    write(DBdata->fdin, msgerror, strlen(msgerror));
    read(DBdata->fdout, str, MAX_BUFF);

    if(strcmp(str,"Error: near line 2: near \"TEST\": syntax error\n")!=0) {
        printf("ERROR CONNECTING DATABASE ERROR OUTPUT\n");
        log(MERROR,"ERROR CONNECTING DATABASE ERROR OUTPUT");
        printf("READ: %s", str);
        return -1;
    }

    printf("Database error output connection successful\n\n");
    log(INFO,"Database error output connection successful");
    return 0;
}
