#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Server/Coms/coms.h"
#include "../Server/Marshalling/marsh.h"
#include "../Server/DB/UUID_DataBase/data_types.h"

#define END_OF_CONNECTION "KILLMEPLZ"

#define MAX_BUF 300


int main() {
    char hostname[MAX_BUF];
    char buffer[MAX_BUF];
    UUIDStock * stock = malloc(sizeof(UUIDStock));
    stock->size = 0;
    stock->last = 0;
    stock->uuids = NULL;

    gethostname(hostname, MAX_BUF);

    gethostname(hostname, 250);
    strcpy(buffer, "12352.");
    strcat(buffer, "localhost");
    connection *con = connect(buffer);

    puts("Price of papa?");
    int pricePapa = getPriceFromDB(con,"papa");
    printf("price of papa %d\n", pricePapa);

    puts("Stock of papa?");
    int stockPapa = getStockFromDB(con,"papa");
    printf("first stock %d\n", stockPapa);


    //START BUY
    puts("Trying to buy 2");
    printf("before buying i got %d papas\n", stock->last);
    int totalPrice,res;
    res = sendBuyTransaction(con, "papa\0", 3, 20, stock,&totalPrice);
    printf("after paying %d, i got %d papas\n",totalPrice, stock->last);

    //END BUY

    puts("Stock of papa?");
    stockPapa = getStockFromDB(con,"papa\0");
    printf("second stock %d\n", stockPapa);

    puts("END TRANSACTION");

	   disconnect(con);


}
