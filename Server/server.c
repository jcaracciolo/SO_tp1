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
#define SEMNAME "semDBss"

dbdata_t* DBdata;
char* addrname;
sem_t* sem;
int msqid;


void createChild(connection * con) {
	int childPID;

	if ((childPID = fork()) == 0) {
		// Child
        openConnection(con);
		assist(con);
		printf("Child fork failed\n");
	} else {
        endConnection(con);
    }
}

void attPriceTransaction(connection * con){
    int client;
    char prodName[MAX_PROD_NAME_LENGHT];

    getRequestedProduct(con,&client,prodName);

    sem_wait(sem);
    int price = getPrice(DBdata, prodName);
    sem_post(sem);

    char buff[MAX_BUF];
    sprintf(buff,"Attending client %d request price ($ %d) for product %s",client,price,prodName);
    log(INFO,buff);

    sendInt(con, price);
}
void attStockTransaction(connection * con){

    int client;
    char prodName[MAX_PROD_NAME_LENGHT];

    getRequestedProduct(con,&client,prodName);

    sem_wait(sem);
    int stock = getStock(DBdata, prodName);
    sem_post(sem);

    char buff[MAX_BUF];
    sprintf(buff,"Attending client %d request stock (%d) for product %s",client,stock,prodName);
    log(INFO,buff);

    sendInt(con, stock);
}
void attBuyTransaction(connection * con){

    char prodName[MAX_PROD_NAME_LENGHT];
    int client,amount,maxPay;

    getBuySellInfo(con,&client,prodName,&amount,&maxPay);

    char buff[MAX_BUF];
    sprintf(buff,"Attending buy trans for client %d, wanting to buy %d of %s at a max of $ %d ",client,amount,prodName,maxPay);
    log(INFO,buff);

    UUIDArray tdata;
    tdata.size=amount;
    pthread_t UUIDthread;

    int err = pthread_create(&(UUIDthread), NULL, &getNUUID, (void *)&tdata);
    if (err != 0) {
        //TODO make something
    }


    sem_wait(sem);
    short buyRealised = 0;
    int price = getPrice(DBdata, prodName);
    int stock = getStock(DBdata, prodName);


    if (price * amount <= maxPay && stock >= amount && amount<=MAX_UUIDS_PER_ARRAY) {

        printf("amount: %i\n", amount);
        printf("old Stock: %i\n", stock);
        updateStock(DBdata, prodName, stock - amount);
        printf("New Stock: %i\n", getStock(DBdata, prodName));

        sprintf(buff,"Buy transaction from %d - %s stock: %d out of %d price: %d out of %d checks out",client,prodName,amount,stock,maxPay,price*amount);
        log(INFO,buff);
        buyRealised = 1;

    } else{
        sem_post(sem);
        if(price * amount > maxPay) {
            sprintf(buff, "Buy transaction from %d - Given money($ %d ) no enough, requires %d",client, maxPay,
                    price * amount);
            log(WARNING, buff);
            sendInt(con,MOREMONEY);
            return MOREMONEY;
        }

        if(stock < amount) {
            sprintf(buff, "Buy transaction from %d - Given stock ( %d ) not enough, required %d",client, stock,amount);
            log(WARNING, buff);
            sendInt(con,STOCK);
            return STOCK;
        }

        if(amount > MAX_UUIDS_PER_ARRAY) {
            sprintf(buff, "Buy transaction from %d - purchase too big (%d). Max amount per purchase: %d",client, amount,MAX_UUIDS_PER_ARRAY);
            log(WARNING, buff);
            sendInt(con,MAXUUIDS);
            return MAXUUIDS;
        }

    }

    sem_post(sem);

    void* ret;
    pthread_join(UUIDthread, &ret);


    if(ret==0 && buyRealised) {
        //tell the client the transaction went through and receive confirmation
        completePurchase(con,&tdata,price*amount);

            //send total amount payed by client
        // sendInt(con,amount*price);
        // sendInt(con,7);
        // fflush(NULL);
        // sendACK(con);
        sprintf(buff, "Buy transaction from %d - %d of %s purchased correctly at %d",client,amount,prodName,amount*price);
        log(INFO, buff);
    }else{
        sendTransType(con,ERROR);
    }
}
void attSellTransaction(connection * con){

    char prodName[MAX_PROD_NAME_LENGHT];
    int client,amount,minPay;

    getBuySellInfo(con,&client,prodName,&amount,&minPay);

    char buff[MAX_BUF];
    sprintf(buff,"Attending sell trans for client %d, wanting to sell %d of %s at a min of $ %d ",client,amount,prodName,minPay);
    log(INFO,buff);

    threadData tdata;
    tdata.n=amount;
    tdata.con=con;

    pthread_t UUIDthread;

    int err = pthread_create(&(UUIDthread), NULL, &readNUUID, (void *)&tdata);
    if (err != 0) {
        //TODO make something
    }

    sem_wait(sem);
    short sellRealised = 0;
    int price = getPrice(DBdata, prodName);

    int stock = getStock(DBdata, prodName);
    if (price * amount >= minPay && amount<=MAX_UUIDS_PER_ARRAY) {

        printf("amount: %i\n", amount);
        printf("old Stock: %i\n", stock);
        updateStock(DBdata, prodName, stock + amount);
        printf("New Stock: %i\n", getStock(DBdata, prodName));
        sprintf(buff,"Sell transaction from %d - %s stock: %d out of %d price: %d out of %d checks out",client,prodName,amount,stock,minPay,price);
        log(INFO,buff);
        sellRealised = 1;
    } else{
        sem_post(sem);
        if(price * amount < minPay) {
            sendInt(con,LESSMONEY);
            sprintf(buff, "Sell transaction from %d - Cannot reach minimal payment($ %d ), cost is  %d",client, minPay,
                    price * amount);
            log(WARNING, buff);
            return LESSMONEY;
        }

        if(amount > MAX_UUIDS_PER_ARRAY) {
            sprintf(buff, "Sell transaction from %d - transaction too big (%d). Max amount per purchase: %d",client, amount,MAX_UUIDS_PER_ARRAY);
            log(WARNING, buff);
            sendInt(con,MAXUUIDS);
            return MAXUUIDS;
        }

    }
    sem_post(sem);

    void* ret;
    pthread_join(UUIDthread, &ret);


    if(ret==0){
        //tell the client the transaction went through and receive confirmation
        sendTransType(con,OK);
        receiveACK(con);
        sendInt(con,price * amount);

        sprintf(buff, "Sell transaction from %d - %d of %s sold correctly at %d",client,amount,prodName,amount*price);
        log(INFO, buff);
    }else{
        sendInt(con,INVALIDUUID);
        sprintf(buff, "Sell transaction from %d - Invalid UUIDS",client);
        log(WARNING, buff);
        return INVALIDUUID;
    }
}


void* getNUUID(UUIDArray* tofill){
    int i;
    for(i=0;i<tofill->size;i++){
        tofill->uuids[i]=getRandomUUID();
    }
    return (void*)0;
}

void* readNUUID(threadData* t){
    sendACK(t->con);
    UUIDStock* recieved=receiveUUIDArray(t->con,t->n);
    int i;

    for(i=0;i<recieved->last;i++){
        if(!UUIDcontains(recieved->uuids[i])){
					printf("%ld - %ld\n",recieved->uuids[i].high,recieved->uuids[i].low);

					return (void*)1;
				}
    }

   return (void*)0;

}



void assist(connection* con) {
    sem=sem_open(SEMNAME,0);

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
                            attSellTransaction(con);
                            break;
                        case BUY:
                            attBuyTransaction(con);
                            break;
                        case CLOSE:
                            printf("finished transaction\n");
                            sem_close(sem);
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

    // Setting signal no to wait for children process
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
    puts("Log connection established\n");


    connectDB(DBdata);

    initializeUUID(UUID_CANT);


    puts("Initializing synchronization");
    //initializing semaphores
    sem=sem_open(SEMNAME,O_CREAT,0600,1);
    if(sem==SEM_FAILED){
        perror("Error initializing synchronization");
        exit(1);
    }

    //Opening server
    puts("Opening server address\n");
    strcpy(addrname,"10.1.34.241:5000/localhost");
    int serverFD = openAdress(addrname);
    if (serverFD < 0) {
        printf("Opening server address failed\n");
		exit(1);
	}

    clock_t begin=clock();
    while (1) {

        sem=sem_open(SEMNAME,0);

    	connection * con = readNewConnection(serverFD);
    	if(con!=NULL){
            log(INFO,"new client connected");
    		createChild(con);
            sem_close(sem);
    	} else if((clock() - begin) > 500 ){
//            printf("\e[1;1H\e[2J");
//            drawChart();
                begin=clock();
                sem_wait(sem);
            //printf("STOCK %d\n",getStock(DBdata,"papa"));
            sem_post(sem);

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
        log(ERROR,"ERROR EXECUTING SQLITE!");
        printf("ERROR EXECUTING SQLITE!");
        exit(1);
    } else {
        printf("Connecting Database...\n\n");
        log(INFO,"Connecting Database...");
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
