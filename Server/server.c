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
#include <poll.h>

#define PATHDBIN "/tmp/fifoDBserverIN"
#define PATHDBOUT "/tmp/fifoDBserverOut"
#define MAX_BUF 300
#define UUID_CANT 100

char SEMNAME[MAX_BUF];

#define TRACKED_PRODUCTS 7
#define TICKS_UNTIL_UPDATE 100

int calculateProdPrice(productPriceData_t * priceData, int currentStock);
int initialiceProduct(productPriceData_t * priceData, char* prodName,
											int initalPrice, int initialStock, int priceAt0Stock);
int printProductPriceData(productPriceData_t * priceData);

dbdata_t* DBdata;
char* addrname;
sem_t* sem;
int msqid;
productPriceData_t  priceData[TRACKED_PRODUCTS];

//This allocates and initialices the pricing information for later use
//when the price needs updating
int initialiceProducts(productPriceData_t * priceData ){
	// priceData = malloc(sizeof(productPriceData_t)*trackedProducts);
	//char* prodName, int initalPrice, int initialStock, int priceAt0Stock
	initialiceProduct(&priceData[0], "papa",30,100,200);
	initialiceProduct(&priceData[1], "tomate",100,50,500);
	initialiceProduct(&priceData[2], "pepino",80,200,1000);
	initialiceProduct(&priceData[3], "cebolla",50,100,250);
	initialiceProduct(&priceData[4], "zucchini",160,60,1000);
	initialiceProduct(&priceData[5], "zapallito",90,150,450);
	initialiceProduct(&priceData[6], "remolacha",40,50,160);

	    // insertIntoTable(DBdata, "papa", 1000, 3);
	    // insertIntoTable(DBdata, "tomate", 1000, 4);
	    // insertIntoTable(DBdata, "pepino", 1000, 5);
	    // insertIntoTable(DBdata, "zanahoria", 1000, 6);
	    // insertIntoTable(DBdata, "remolacha", 1000, 7);
	    // insertIntoTable(DBdata, "zapallito", 1000, 8);
	    // insertIntoTable(DBdata, "zucchini", 1000, 9);

}

int initialiceProduct(productPriceData_t * priceData, char* prodName,
											int initialPrice, int initialStock, int priceAt0Stock){

		strcpy(priceData->prodName,prodName);
		priceData->initialPrice = initialPrice;
		priceData->initialStock = initialStock;
		priceData->priceAt0Stock = priceAt0Stock;
		insertIntoTable(DBdata, prodName, initialStock, initialPrice);
}
void createChild(connection * con) {
	int childPID;

	if ((childPID = fork()) == 0) {
		// Child
		assist(con);
		printf("Child fork failed\n");
	} else {
        endConnection(con);
        freeCon(con);
    }
}

void attExistsTransaction(connection * con) {
    int client;
    char prodName[MAX_PROD_NAME_LENGHT];

    getRequestedProduct(con,&client,prodName);

    sem_wait(sem);
    int exists = existsInDB(DBdata, prodName);
    sem_post(sem);

    char buff[MAX_BUF];
    sprintf(buff,"Attending client %d request existence of product %s",client,prodName);
    msglog(INFO,buff);

    sendInt(con, exists);
}

void attPriceTransaction(connection * con) {
    int client;
    char prodName[MAX_PROD_NAME_LENGHT];

    getRequestedProduct(con,&client,prodName);

    sem_wait(sem);
    int price = getPrice(DBdata, prodName);
    sem_post(sem);

    char buff[MAX_BUF];
    sprintf(buff,"Attending client %d request price ($ %d) for product %s",client,price,prodName);
    msglog(INFO,buff);

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
    msglog(INFO,buff);

    sendInt(con, stock);
}
void attBuyTransaction(connection * con){

    char prodName[MAX_PROD_NAME_LENGHT];
    int client,amount,maxPay;

    getBuySellInfo(con,&client,prodName,&amount,&maxPay);

    char buff[MAX_BUF];
    sprintf(buff,"Attending buy trans for client %d, wanting to buy %d of %s at a max of $ %d ",client,amount,prodName,maxPay);
    msglog(INFO,buff);


    if(amount > MAX_UUIDS_PER_ARRAY) {
        sprintf(buff, "Buy transaction from %d - purchase too big (%d). Max amount per purchase: %d",client, amount,MAX_UUIDS_PER_ARRAY);
        msglog(WARNING, buff);
        sendInt(con,MAXUUIDS);
        return;
    }


    UUIDArray tdata;
    tdata.size=amount;
    pthread_t UUIDthread;


    int err = pthread_create(&(UUIDthread), NULL,(void * (*)(void *)) &getNUUID, (void *)&tdata);
    if (err != 0) {
        msglog(MERROR, "ERROR CREATING A THREAD, ASSISTANT MUST EXIT");
        sendInt(con,NOCONECTION);
        //TODO liberar cosas
        exit(0);
    }


    sem_wait(sem);

    if(!existsInDB(DBdata,prodName)){
        sem_post(sem);
        sprintf(buff, "Buy transaction from %d - %s not a valid element",client,prodName);
        msglog(WARNING, buff);
        sendInt(con,NOSUCHELEMENT);
        return;
    }

    int price = getPrice(DBdata, prodName);
    int stock = getStock(DBdata, prodName);

    sprintf(buff,"Buy transaction from %d - %s stock: %d out of %d price: %d",client,prodName,amount,stock,price);
    msglog(INFO,buff);

    if (price * amount <= maxPay && stock >= amount && amount<=MAX_UUIDS_PER_ARRAY) {

        updateStock(DBdata, prodName, stock - amount);

        sprintf(buff,"Buy transaction from %d - %s stock: %d out of %d price: %d out of %d checks out",client,prodName,amount,stock,maxPay,price*amount);
        msglog(INFO,buff);

    } else{
        sem_post(sem);
        if(price * amount > maxPay) {
            sprintf(buff, "Buy transaction from %d - Given money($ %d ) no enough, requires %d",client, maxPay,
                    price * amount);
            msglog(WARNING, buff);
            sendInt(con,MOREMONEY);
            return;
        }

        if(stock < amount) {
            sprintf(buff, "Buy transaction from %d - Given stock ( %d ) not enough, required %d",client, stock,amount);
            msglog(WARNING, buff);
            sendInt(con,NOSTOCK);
            return;
        }

    }

    sem_post(sem);

    void* ret;
    pthread_join(UUIDthread, &ret);


    //tell the client the transaction went through and receive confirmation
    if(completePurchase(con,&tdata,price*amount)==NOCONECTION)
        return;

    sprintf(buff, "Buy transaction from %d - %d of %s purchased correctly at %d",client,amount,prodName,amount*price);
    msglog(INFO, buff);

}
void attSellTransaction(connection * con){

    char prodName[MAX_PROD_NAME_LENGHT];
    int client,amount,minPay;

    getBuySellInfo(con,&client,prodName,&amount,&minPay);

    char buff[MAX_BUF];
    sprintf(buff,"Attending sell trans for client %d, wanting to sell %d of %s at a min of $ %d ",client,amount,prodName,minPay);
    msglog(INFO,buff);


    if(amount > MAX_UUIDS_PER_ARRAY) {
        sprintf(buff, "Sell transaction from %d - transaction too big (%d). Max amount per purchase: %d",client, amount,MAX_UUIDS_PER_ARRAY);
        msglog(WARNING, buff);
        sendInt(con,MAXUUIDS);
        return;
    }

    threadData tdata;
    tdata.n=amount;
    tdata.con=con;

    pthread_t UUIDthread;

    int err = pthread_create(&(UUIDthread), NULL,(void * (*)(void *)) &readNUUID, (void *)&tdata);
    if (err != 0) {
        msglog(MERROR, "ERROR CREATING A THREAD, ASSISTANT MUST EXIT");
        sendInt(con,NOCONECTION);
        //TODO liberar cosas
        exit(0);
    }

    sem_wait(sem);

    if(!existsInDB(DBdata,prodName)){
        sem_post(sem);
        sprintf(buff, "Buy transaction from %d - %s not a valid element",client,prodName);
        msglog(WARNING, buff);
        sendInt(con,NOSUCHELEMENT);
        return;
    }

    int price = getPrice(DBdata, prodName);
    int stock = getStock(DBdata, prodName);

    void* ret;
    pthread_join(UUIDthread, &ret);

    if (price * amount >= minPay && ret==0) {

        printf("amount: %i\n", amount);
        printf("old Stock: %i\n", stock);

        updateStock(DBdata, prodName, stock + amount);
        sem_post(sem);

        printf("New Stock: %i\n", getStock(DBdata, prodName));

        sprintf(buff,"Sell transaction from %d - %s stock: %d out of %d price: %d out of %d checks out",client,prodName,amount,stock,minPay,price);
        msglog(INFO,buff);


        sendTransType(con,SUCCESS);
        receiveACK(con);
        sendInt(con,price * amount);

        sprintf(buff, "Sell transaction from %d - %d of %s sold correctly at %d",client,amount,prodName,amount*price);
        msglog(INFO, buff);

        return;
    }else{
        sem_post(sem);
        if(price * amount < minPay) {
            sendInt(con,LESSMONEY);
            sprintf(buff, "Sell transaction from %d - Cannot reach minimal payment($ %d ), cost is  %d",client, minPay,
                    price * amount);
            msglog(WARNING, buff);
            return;
        }

        if(ret!=0){
            sendInt(con,INVALIDUUID);
            sprintf(buff, "Sell transaction from %d - Invalid UUIDS",client);
            msglog(WARNING, buff);
            return;
        }

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
					printf("%ld - %ld\n",(long int)recieved->uuids[i].high,(long int)recieved->uuids[i].low);
					return (void*)1;
				}
    }

   return (void*)0;

}
void attCloseTransaction(connection* con){
    printf("finished transaction\n");
    msglog(INFO,"Client disconnected");
    sem_close(sem);
    endConnection(con);
    // Closing assistant
    // close(con)

}

void deathHandler(int signo){
    sem_close(sem);
    msglog(MERROR, "Master server died");
    exit(0);
}


void assist(connection* con) {

    //In charge of releasing resources in case parent dies
    struct sigaction sigchld_action = {
            .sa_handler = &deathHandler
    };
    sigaction(SIGHUP, &sigchld_action, NULL);

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
                        case EXISTS:
                            attExistsTransaction(con);
                            break;
                        case CLOSE:
                            attCloseTransaction(con);
                            freeCon(con);
                            exit(0);
                    }

    }
}



void msglog(int priority,char* message){
    msgbuf_t msg={ .mtype=0,.message={0} };
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
    char logPath[MAX_BUF];
    int a = readLogFromConfigFile(logPath);

    if((f=fork())==0){
        char* ar[3]={"log",logPath,NULL};
        execv("log",ar);
        puts("Log failed to execute");
        free(DBdata);
        free(addrname);
        exit(1);
    }else if(f<0){
        perror(logPath);
        free(DBdata);
        free(addrname);
        exit(1);
    }

    int key = ftok("DO_NOT_DELETE",'A');
    if ((msqid = msgget(key, 0644)) == -1) {
        perror("ERROR CONNECTING LOG");
        free(DBdata);
        free(addrname);
        exit(1);
    }

    msglog(INFO,"open log");
    msglog(INFO,"Log created");
    puts("Log connection established\n");



    if(connectDB(DBdata)==-1){
        msglog(INFO,"end of log");
        free(DBdata);
        free(addrname);
        exit(1);
    }

    initializeUUID(UUID_CANT);


    puts("Initializing synchronization");
    //initializing semaphores
    
    if (!readSemNameFromConfigFile(SEMNAME)) {
        perror("Error initializing synchronization");
        msglog(MERROR,"Error initializing synchronization");
        msglog(INFO,"end of log");
        exitDB(DBdata);
        exit(1);
    }

    sem=sem_open(SEMNAME,O_CREAT,0600,1);

    if(sem==SEM_FAILED){
        perror("Error initializing synchronization");
        msglog(MERROR,"Error initializing synchronization");
        msglog(INFO,"end of log");
        exitDB(DBdata);
        exit(1);
    }

    //Opening server
    puts("Opening server address\n");

    if (readAddrFromConfigFile("hostAddress.info", addrname)) {
        puts("Failed reading address in configuration file");
        msglog(MERROR, "Failed openning server's address.");
        sem_close(sem);
        sem_unlink(SEMNAME);
        msglog(INFO,"end of log");
        exitDB(DBdata);
        free(DBdata);
        free(addrname);
        exit(1);
    }

    msglog(INFO,"Opening Server address");
    int serverFD = openAdress(addrname);
    if (serverFD < 0) {
        printf("Opening server address failed\n");
        msglog(MERROR,"Opening server address failed");
        sem_close(sem);
        sem_unlink(SEMNAME);
        msglog(INFO,"end of log");
        exitDB(DBdata);
        free(DBdata);
        free(addrname);
        exit(1);
	}

    puts("Server open. Listening...\n");


		// printProductPriceData(&priceData[0]);
		// printProductPriceData(&priceData[1]);
    clock_t begin=clock();
		int ticks=0;
    while (1) {

        sem=sem_open(SEMNAME,0);

    	connection * con = readNewConnection(serverFD);

        if(con!=NULL){
            msglog(INFO,"new client connected");
    		createChild(con);
            sem_close(sem);

        } else if((clock() - begin) > 500 ){
            struct pollfd poll_list[2];

            poll_list[0].fd = 0;
            poll_list[0].events = POLLIN;

            // poll checks if something was sent to srdin
            int readSmth = poll(poll_list, (unsigned long) 1, 10);

            if(readSmth) {
                char smth[MAX_BUF] = {0};
                read(0, smth, 1);
                smth[1] = '\0';
                if(strcmp(smth, "q") == 0) {
                    break;

                }
                while(getchar() !=  EOF);
            }

            if(ticks++ >= TICKS_UNTIL_UPDATE){
                int i,stock=0,price=0, oldPrice=0;
                productPriceData_t * selectedPriceData;
                sem_wait(sem);
                for (i = 0; i < TRACKED_PRODUCTS ; i++) {
                    selectedPriceData = &priceData[i];
                    // printProductPriceData(selectedPriceData);
                    stock = getStock(DBdata,selectedPriceData->prodName);
                    oldPrice = getPrice(DBdata,selectedPriceData->prodName);
                    price = calculateProdPrice(selectedPriceData, stock);
                    updatePrice(DBdata, selectedPriceData->prodName, price);
                    printf("Updating price of %ss from %d to %d\n",
                                selectedPriceData->prodName,oldPrice,price);
                }
                sem_post(sem);
                printf("\n\n");
                ticks = 0;
            }
        }
    }

    puts("Closing server...");
    kill(0,SIGUSR1);
    sem_close(sem);
    sem_unlink(SEMNAME);
    msglog(INFO,"end of log");
    exitDB(DBdata);
    free(DBdata);
    free(addrname);
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
		initialiceProducts(priceData);

    // insertIntoTable(DBdata, "papa", 1000, 3);
    // insertIntoTable(DBdata, "tomate", 1000, 4);
    // insertIntoTable(DBdata, "pepino", 1000, 5);
    // insertIntoTable(DBdata, "zanahoria", 1000, 6);
    // insertIntoTable(DBdata, "remolacha", 1000, 7);
    // insertIntoTable(DBdata, "zapallito", 1000, 8);
    // insertIntoTable(DBdata, "zucchini", 1000, 9);
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
        msglog(MERROR,"ERROR EXECUTING SQLITE!");
        printf("ERROR EXECUTING SQLITE!");
        return -1;
    } else {
        printf("Connecting Database...\n\n");
        msglog(INFO,"Connecting Database...");
        DBdata->fdin = open(PATHDBIN,O_WRONLY);
        DBdata->fdout = open(PATHDBOUT,O_RDONLY);

        if(checkDBConnection(DBdata)==-1)
            return -1;

        initializeDB(DBdata);

    }

    return 0;

}

void initializeUUID(unsigned int n){
    int i;
    printf("Initializing UUID database...\n\n");
    msglog(INFO,"Initializing UUID database...");
    for(i=0;i<n;i++){
        printf("\rIn progress [%.*s%*s] %.2f %%",(i+1)*30/n,"||||||||||||||||||||||||||||||||||||||||",30-(i+1)*30/n,"", (i*100.0)/n);
        fflush(stdout);
        UUIDadd(newUUID((uint64_t) random(),(uint64_t) random()));
    }
    msglog(INFO,"UUID insertion succesful");
    printf("\nUUID insertion succesful\n");
}




int printProductPriceData(productPriceData_t * priceData){
	printf("Pricing data for %s\n", priceData->prodName);
	printf("initPrice: %d initStock: %d\n", priceData->initialPrice,priceData->initialStock);
	printf("priceAt0Stock: %d\n",priceData->priceAt0Stock);
}

int calculateProdPrice(productPriceData_t * priceData, int currentStock){
	float m = (priceData->initialPrice-priceData->priceAt0Stock)/(1.0*priceData->initialStock);
	// printf("m: %f\n", m);
	int x =  m * currentStock + priceData->priceAt0Stock;
	if(x <= 0) return 1;
	return x;
}
