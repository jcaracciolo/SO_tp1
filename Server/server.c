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
char* addrname;
sem_t* semid;


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
		sendACK(con);
		receiveString(con, prodName,MAX_PROD_NAME_LENGHT);
		int price = getPrice(DBdata, prodName);
		sendInt(con, price);
}



void attBuyTransaction(connection * con){

    printf("Attending purchase\n");
    char prodName[MAX_PROD_NAME_LENGHT];

    sendACK(con);

		//Receive prodname and send ack
    receiveString(con, prodName,MAX_PROD_NAME_LENGHT);
		printf("prodName %s\n", prodName);
    sendACK(con);

		//receive amount of product to buy
    int amount=receiveInt(con);
		printf("cantidad %d\n", amount);
		sendACK(con);

		//receive max price the client is willing to pay
    int maxPay=receiveInt(con);
		printf("max price %d\n", maxPay);

    UUIDArray tdata;
    tdata.size=amount;
    pthread_t UUIDthread;

    int err = pthread_create(&(UUIDthread), NULL, &getNUUID, &tdata);
    if (err != 0) {
        //TODO make something
    }

    sem_t* semid=sem_open(addrname,0);
    sem_wait(semid);

    //TODO get from DB
    short buyRealised = 0;
    int price = getPrice(DBdata, prodName);
		printf("price per papa %d\n", price);
    int stock = getStock(DBdata, prodName);
		//Check if the client is not overpaying and is not trying to buy too much
    if (price * amount <= maxPay && stock >= amount && amount<=MAX_UUIDS_PER_ARRAY) {
            printf("amount: %i\n", amount);
            printf("old Stock: %i\n", stock);
            updateStock(DBdata, prodName, stock - amount);
            printf("New Stock: %i\n", getStock(DBdata, prodName));
            buyRealised = 1;
    } else printf("You want to pay too little or buy too much\n");

    //TODO thread get from UUID
    sem_post(semid);
    sem_close(semid);

    int ret;
    pthread_join(UUIDthread, &ret);

    if(ret==0 && buyRealised){
				//tell the client the transaction went through and receive confirmation
				sendTransType(con,OK);
				receiveACK(con);

        sendUUIDArray(con,&tdata,price*amount);
				//send total amount payed by client
        // sendInt(con,amount*price);
        // sendInt(con,7);
				// fflush(NULL);
				// sendACK(con);
				puts("sent total\n");
    }else{
        sendTransType(con,ERROR);
    }
		receiveACK(con);
}

int getNUUID(UUIDArray* tofill){
    for(int i=0;i<tofill->size;i++){
        tofill->uuids[i]=getRandomUUID();
    }
    return 0;
}

void attStockTransaction(connection * con){
		printf("Attending stock\n");
		char prodName[MAX_PROD_NAME_LENGHT];
		sendACK(con);
		receiveString(con, prodName,MAX_PROD_NAME_LENGHT);
		int price = getStock(DBdata, prodName);
		sendInt(con, price);
}

int validateUUID(char* arg){

    printf("THIS THREAD SAID: %s\n",arg);
    return 1;
}

void assist(connection* con) {
    while (1) {

                pthread_t UUIDthread;
                int err;
				transType_t transactionType=receiveTransType(con);
                    switch (transactionType) {
                        case PRICE:
                            attPriceTransaction(con);
                            break;
                        case STOCK:
                            attStockTransaction(con);
                            break;
                        case SELL:

                            if (err != 0) {
                                //TODO make something
                            }
                            int ret;

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

    srand(0);

    DBdata=malloc(sizeof(dbdata_t));
    addrname=calloc(MAX_BUF,1);

    connectDB(DBdata);
    initializeUUID(UUID_CANT);

    puts("Initializing synchronization");

    sem_t* sem;
    strcpy(addrname,"12352.");
    strcat(addrname,"localhost");

    sem=sem_open(addrname,O_CREAT,0600,1);

    if(sem==SEM_FAILED){
        perror("Error initializing synchronization");
        exit(1);
    }

	int serverFD = openAdress(addrname);
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
