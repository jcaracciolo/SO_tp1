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


int main() {
    char hostname[MAX_BUF];
    char buffer[MAX_BUF];


    gethostname(hostname, MAX_BUF);

    gethostname(hostname, 250);
    strcpy(buffer, "12352.");
    strcat(buffer, "localhost");
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

    for(int i=0;i<ans->last;i++){
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
