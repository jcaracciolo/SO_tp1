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

    puts("Price of papa?");
    int pricePapa;
    sendInt(con, PRICE);
    int ackn = receiveInt(con); //TODO: replace with ack
    if(ackn!=ACKNOWLEDGE){
        puts("ERROR");
        exit(1);
    }
    sendString(con, "papa\0");
    pricePapa = receiveInt(con);
    printf("%d\n", pricePapa);

    puts("Stock of papa?");
    int onestock;
    sendInt(con, STOCK);
    ackn = receiveInt(con); //TODO: replace with ack
    if(ackn!=ACKNOWLEDGE){
        puts("ERROR");
        exit(1);
    }
    sendString(con, "papa\0");
    onestock = receiveInt(con);
    printf("first stock %d\n", onestock);

    sendInt(con,BUY);
    ackn = receiveInt(con); //TODO: replace with ack
    if(ackn!=ACKNOWLEDGE){
        puts("ERROR");
        exit(1);
    }

    sendString(con, "papa\0");
    ackn = receiveInt(con); //TODO: replace with ack
    if(ackn!=ACKNOWLEDGE){
        puts("ERROR");
        exit(1);
    }


    sendInt(con,2);
    ackn = receiveInt(con); //TODO: replace with ack
    if(ackn!=ACKNOWLEDGE){
        puts("ERROR");
        exit(1);
    }

    sendInt(con,3*pricePapa);

    int cost;
    UUIDStock *ans=receiveUUIDArray(con,2,&cost);

    int i;
    for(i=0;i<ans->last;i++){
        printf("%ld - %ld\n",ans->uuids[i].high,ans->uuids[i].low);
    }
    printf("%d\n",cost);

     puts("Stock of papa?");
    int secondstock;
    sendInt(con, STOCK);
    ackn = receiveInt(con); //TODO: replace with ack
    if(ackn!=ACKNOWLEDGE){
        puts("ERROR");
        exit(1);
    }
    sendString(con, "papa\0");
    secondstock = receiveInt(con);
    printf("second stock %d\n", secondstock);

    puts("END TRANSACTION");

	sendInt(con, CLOSE);
    endConnection(con);


}
