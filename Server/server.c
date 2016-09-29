#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "Coms/coms.h"
#include "server.h"
#include "DB/SQlite/SQLparser.h"
#include "DB/UUID_DataBase/DB.h"
#include "Marshalling/marsh.h"

#define PATHDBIN "/tmp/fifoDBserverIN"
#define PATHDBOUT "/tmp/fifoDBserverOut"

#define MAX_BUF 300
#define UUID_CANT 100

dbdata_t* DBdata;


void createChild(connection * con) {
	int childPID;

	if ((childPID = fork()) == 0) {
		// Child
		assist(con);
		printf("Child fork failed\n");
	}
}

void attPriceTransaction(connection * con){
		printf("Attending price\n");
		char prodName[MAX_PROD_NAME_LENGHT];

		sendInt(con,ACKNOWLEDGE);
		receiveString(con, prodName,MAX_PROD_NAME_LENGHT);

		int price = getPrice(DBdata, prodName);
		sendInt(con, price);
}

void attBuyTransaction(connection * con){
    printf("Attending purchase\n");
    char prodName[MAX_PROD_NAME_LENGHT];

    sendInt(con,ACKNOWLEDGE);

    receiveString(con, prodName,MAX_PROD_NAME_LENGHT);

    int amount;
    sendInt(con,ACKNOWLEDGE);
    amount=receiveInt(con);

    int maxPay;
    sendInt(con,ACKNOWLEDGE);
    maxPay=receiveInt(con);

    //TODO mutex, if max pay>cancel
    //TODO get from DB
    //TODO thread get from UUID

    UUIDArray msg;
    for(int i=0;i<amount;i++){
        msg.uuids[i]=getRandomUUID();
    }
    msg.size=amount;


    sendUUIDArray(con,&msg);
    receiveInt(con);
    sendInt(con,amount*3);

}

void attStockTransaction(connection * con){
		printf("Attending stock\n");
		char prodName[MAX_PROD_NAME_LENGHT];
		sendInt(con,9); //TODO replace with ack
		receiveString(con, prodName,MAX_PROD_NAME_LENGHT);
		int stock = getPrice(DBdata, prodName);
		sendInt(con, stock);
}
int validateUUID(char* arg){

    printf("THIS THREAD SAID: %s\n",arg);
    return 1;
}

void assist(connection* con) {
    while (1) {

                pthread_t UUIDthread;
                int err;
				int transactionType=receiveInt(con);
                    switch (transactionType) {
                        case PRICE:
                            attPriceTransaction(con);
                            break;
                        case STOCK:
                            attStockTransaction(con);
                            break;
                        case SELL:
                            err = pthread_create(&(UUIDthread), NULL, &validateUUID, "HOLA");
                            if (err != 0) {
                                //TODO make something
                            }
                            int ret;
                            pthread_join(UUIDthread, &ret);
                            printf("thread done %d\n", ret);
                            break;
                        case BUY:
                            attBuyTransaction(con);
                            break;
                        case CLOSE:
                            printf("finished transaction\n");
                            endConnection(con);
                            // Closing assistant
                            // close(con)
                            exit(0);
                        default:
                            printf("CLIENT PID %d\n\n", transactionType);
                            break;

                    }

				// printf("%s thsi %d\n",buf,3);

				// UUIDArray uuarr;
				// uuarr.uuids[0].high = 1111111;
				// uuarr.uuids[1].high = 222222222;
				// uuarr.uuids[0].low = 3333333;
				// uuarr.uuids[1].low = 5555555;
				// uuarr.size = 2;
				//
				// sendUUIDArray(con, &uuarr);


    }
}


int main(int argc, char *argv[]) {
    char hostname[MAX_BUF];
    char buffer[MAX_BUF];
    srand(0);

    DBdata=malloc(sizeof(dbdata_t));
    connectDB(DBdata);
    initializeUUID(UUID_CANT);

	gethostname(hostname,MAX_BUF);

	strcpy(buffer,"12352.");
	strcat(buffer,hostname);

	int serverFD = openAdress(buffer);
	if (serverFD < 0) {
		printf("Opening server address failed\n");
		exit(1);
	}

    while (1) {
    	connection * con = readNewConnection(serverFD);
    	if(con!=NULL){
    		createChild(con);
    	} else {
            // server do his stuff...
        }
    }

	return 0;
}

void initializeDB(dbdata_t * DBdata) {
    createTable(DBdata);
    insertIntoTable(DBdata, "papa", 8, 3);
}

int connectDB(dbdata_t* DBdata){

    mkfifo(PATHDBOUT,0666);
    mkfifo(PATHDBIN,0666);

    if (fork() == 0) {
        //child DATABASE
        close(0);
        close(1);
        close(2);
        open(PATHDBIN,O_RDONLY);
        open(PATHDBOUT,O_WRONLY);
        open(PATHDBOUT,O_WRONLY);

        char* ar[3]={"sqlite3","-echo",NULL};
        execv("./DB/SQlite/sqlite3",ar);
        printf("ERROR EXECUTING SQLITE!");
        exit(1);
    } else {
        printf("Connecting Database...\n\n");

        DBdata->fdin = open(PATHDBIN,O_WRONLY);
        DBdata->fdout = open(PATHDBOUT,O_RDONLY);

        checkDBConnection(DBdata);
        initializeDB(DBdata);

    }

    return 0;

}

void initializeUUID(unsigned int n){
    int i;
    printf("Initializing UUID database...\n\n");
    for(i=0;i<n;i++){
        printf("\rIn progress [%.*s%*s] %.2f %%",(i+1)*30/n,"||||||||||||||||||||||||||||||||||||||||",30-(i+1)*30/n,"", (i*100.0)/n);
        fflush(stdout);
        UUIDadd(newUUID((uint64_t) random(),(uint64_t) random()));
    }
    printf("\nUUID insertion succesful\n");
}
