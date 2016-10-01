#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include "Coms/coms.h"
#include "server.h"
#include "DB/SQlite/SQLparser.h"
#include "DB/UUID_DataBase/DB.h"
#include "Marshalling/marsh.h"
#include "Logs/log.h"

#define PATHDBIN "/tmp/fifoDBserverIN"
#define PATHDBOUT "/tmp/fifoDBserverOut"
#define MAX_BUF 300
#define UUID_CANT 100
#define SEMNAME "sem"

dbdata_t* DBdata;
char* addrname;
sem_t* semid;
int msqid;




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

    int err = pthread_create(&(UUIDthread), NULL, &getNUUID, (void *)&tdata);
    if (err != 0) {
        //TODO make something
    }

    sem_t* semid=sem_open(SEMNAME,0);
    sem_wait(semid);

    //TODO get from DB
    short buyRealised = 0;
    int price = getPrice(DBdata, prodName);
		printf("price per papa %d\n", price);
    int stock = getStock(DBdata, prodName);
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
    int i;
    for(i=0;i<tofill->size;i++){
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

void log(int priority,char* message){
    msgbuf_t msg;
    msg.mtype=priority;
    strcpy(msg.message,message);
    msgsnd(msqid,&msg,sizeof(msg),0);
}

int main(int argc, char *argv[]) {
    DBdata=malloc(sizeof(dbdata_t));
    addrname=calloc(MAX_BUF,1);
    char hostname[MAX_BUF];

    srand(0);

    struct sigaction sigchld_action = {
            .sa_handler = SIG_DFL,
            .sa_flags = SA_NOCLDWAIT
    };
    sigaction(SIGCHLD, &sigchld_action, NULL);

    int f;
    puts("Initializing log");
    if((f=fork())==0){
        char* ar[3]={"log","DO_NOT_DELETE",NULL};
        execv("log",ar);
    }else if(f<0){
        perror("ERROR CONNECTING LOG");
        exit(0);
    }

    int key = ftok("DO_NOT_DELETE",'A');
    if ((msqid = msgget(key, 0644)) == -1) {
        perror("ERROR CONNECTING LOG");
        exit(0);
    }
    log(INFO,"open log");

    log(INFO,"Log created");
    puts("Log connection established");

    connectDB(DBdata);
    log(INFO,"Connection to Database");
    initializeUUID(UUID_CANT);

    puts("Initializing synchronization");

    sem_t* sem;
    sem=sem_open(SEMNAME,O_CREAT,0600,1);
    if(sem==SEM_FAILED){
        perror("Error initializing synchronization");
        exit(1);
    }

    strcpy(addrname,"10.1.34.241:5000/localhost");
    int serverFD = openAdress(addrname);
    if (serverFD < 0) {
        printf("Opening server address failed\n");
		exit(1);
	}

    clock_t begin=clock();
    while (1) {
    	connection * con = readNewConnection(serverFD);
    	if(con!=NULL){
    		createChild(con);
    	} else if((clock() - begin) > 500 ){
//            printf("\e[1;1H\e[2J");
//            drawChart();
//            begin=clock();
//
        }
    }

	return 0;
}

void drawChart(){
    int stock=getStock(DBdata,"papa");
    int price=getPrice(DBdata,"papa");
    printf("papas | %d | %d ",stock,price);
    int i;
    for(i=0;i<stock;i++){
        printf("----");
    }
    puts("");


}

void initializeDB(dbdata_t * DBdata) {
    createTable(DBdata);
    insertIntoTable(DBdata, "papa", 8000000, 3);
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
