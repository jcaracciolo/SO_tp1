#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Server/Coms/coms.h"
#include "../Server/Marshalling/marsh.h"
#include "../Server/DB/UUID_DataBase/data_types.h"

#define END_OF_CONNECTION "KILLMEPLZ"

#define MAX_BUF 300



int main(int argc, int * argv[]) {
    char buffer[MAX_BUF];

    if (argc == 1) {        
    strcpy(buffer, ":5000/localhost");
    } else if (argc == 2) {
        strcpy(buffer, argv[1]); //10.1.34.241
        strcat(buffer, ":5000/localhost");
    } else {
        puts("Invalid quantity of arguments");
        exit(1);
    }

    puts(buffer);

    connection *con = connectToAddres(buffer);

    UUIDStock * stock = malloc(sizeof(UUIDStock));
    stock->size = 0;
    stock->last = 0;
    stock->uuids = NULL;

    puts("Price of papa?");
    int pricePapa = getPriceFromDB(con,"papa");
    printf("adada %d\n", pricePapa);

    puts("Stock of papa?");
    int stockPapa = getStockFromDB(con,"papa");
    printf("first stock %d\n", pricePapa);

    puts("Stock of papa?");
    stockPapa = getStockFromDB(con,"papa");
    printf("first stock %d\n", pricePapa);

    //START BUY
    puts("Trying to buy 2");
    printf("before buying i got %d papas\n", stock->last);
    int res = sendBuyTransaction(con, "papa\0", 2, 10, stock);
    printf("after buying i got %d papas\n", stock->last);

    //END BUY

    puts("Stock of papa?");
    stockPapa = getStockFromDB(con,"papa\0");
    printf("second stock %d\n", pricePapa);

    puts("END TRANSACTION");

	   disconnect(con);


}
