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
#define SEMNAME "semddmBss"



int calculateProdPrice(productPriceData_t * priceData, int currentStock);
int initialiceProduct(productPriceData_t * priceData, char* prodName,
											int initalPrice, int initialStock, int priceAt0Stock);
dbdata_t* DBdata;
char* addrname;
sem_t* sem;
int msqid;


void createChild(connection * con) {
	int childPID;

	if ((childPID = fork()) == 0) {
		// Child
        close(2);
        close(3);
        openConnection(con);
		assist(con);
		printf("Child fork failed\n");
	} else {
        endConnection(con);
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
    log(INFO,buff);

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


    if(amount > MAX_UUIDS_PER_ARRAY) {
        sprintf(buff, "Buy transaction from %d - purchase too big (%d). Max amount per purchase: %d",client, amount,MAX_UUIDS_PER_ARRAY);
        log(WARNING, buff);
        sendInt(con,MAXUUIDS);
        return MAXUUIDS;
    }


    UUIDArray tdata;
    tdata.size=amount;
    pthread_t UUIDthread;


    int err = pthread_create(&(UUIDthread), NULL, &getNUUID, (void *)&tdata);
    if (err != 0) {
        sprintf(buff, "ERROR CREATING A THREAD, ASSISTANT MUST EXIT",client,prodName);
        log(MERROR, buff);
        sendInt(con,NOCONECTION);
        //TODO liberar cosas
        exit(0);
    }


    sem_wait(sem);

    if(!existsInDB(DBdata,prodName)){
        sem_post(sem);
        sprintf(buff, "Buy transaction from %d - %s not a valid element",client,prodName);
        log(WARNING, buff);
        sendInt(con,NOSUCHELEMENT);
        return NOSUCHELEMENT;
    }

    int price = getPrice(DBdata, prodName);
    int stock = getStock(DBdata, prodName);

    sprintf(buff,"Buy transaction from %d - %s stock: %d out of %d price: %d",client,prodName,amount,stock,price);
    log(MERROR,buff);

    if (price * amount <= maxPay && stock >= amount && amount<=MAX_UUIDS_PER_ARRAY) {

        updateStock(DBdata, prodName, stock - amount);

        sprintf(buff,"Buy transaction from %d - %s stock: %d out of %d price: %d out of %d checks out",client,prodName,amount,stock,maxPay,price*amount);
        log(INFO,buff);

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
            sendInt(con,NOSTOCK);
            return STOCK;
        }

    }

    sem_post(sem);

    void* ret;
    pthread_join(UUIDthread, &ret);


    //tell the client the transaction went through and receive confirmation
    if(completePurchase(con,&tdata,price*amount)==NOCONECTION)
        return NOCONECTION;

    sprintf(buff, "Buy transaction from %d - %d of %s purchased correctly at %d",client,amount,prodName,amount*price);
    log(INFO, buff);

}
void attSellTransaction(connection * con){

    char prodName[MAX_PROD_NAME_LENGHT];
    int client,amount,minPay;

    getBuySellInfo(con,&client,prodName,&amount,&minPay);

    char buff[MAX_BUF];
    sprintf(buff,"Attending sell trans for client %d, wanting to sell %d of %s at a min of $ %d ",client,amount,prodName,minPay);
    log(INFO,buff);


    if(amount > MAX_UUIDS_PER_ARRAY) {
        sprintf(buff, "Sell transaction from %d - transaction too big (%d). Max amount per purchase: %d",client, amount,MAX_UUIDS_PER_ARRAY);
        log(WARNING, buff);
        sendInt(con,MAXUUIDS);
        return MAXUUIDS;
    }

    threadData tdata;
    tdata.n=amount;
    tdata.con=con;

    pthread_t UUIDthread;

    int err = pthread_create(&(UUIDthread), NULL, &readNUUID, (void *)&tdata);
    if (err != 0) {
        sprintf(buff, "ERROR CREATING A THREAD, ASSISTANT MUST EXIT",client,prodName);
        log(MERROR, buff);
        sendInt(con,NOCONECTION);
        //TODO liberar cosas
        exit(0);
    }

    sem_wait(sem);

    if(!existsInDB(DBdata,prodName)){
        sem_post(sem);
        sprintf(buff, "Buy transaction from %d - %s not a valid element",client,prodName);
        log(WARNING, buff);
        sendInt(con,NOSUCHELEMENT);
        return NOSUCHELEMENT;
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
        log(INFO,buff);


        sendTransType(con,OK);
        receiveACK(con);
        sendInt(con,price * amount);

        sprintf(buff, "Sell transaction from %d - %d of %s sold correctly at %d",client,amount,prodName,amount*price);
        log(INFO, buff);

        return OK;
    }else{
        sem_post(sem);
        if(price * amount < minPay) {
            sendInt(con,LESSMONEY);
            sprintf(buff, "Sell transaction from %d - Cannot reach minimal payment($ %d ), cost is  %d",client, minPay,
                    price * amount);
            log(WARNING, buff);
            return LESSMONEY;
        }

        if(ret!=0){
            sendInt(con,INVALIDUUID);
            sprintf(buff, "Sell transaction from %d - Invalid UUIDS",client);
            log(WARNING, buff);
            return INVALIDUUID;
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
                        case EXISTS:
                            attExistsTransaction(con);
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
    if((f=fork())==0){
        char* ar[3]={"log","DO_NOT_DELETE",NULL};
        execv("log",ar);
    }else if(f<0){
        perror("ERROR EXECUTING LOG");
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
        log(MERROR,"Error initializing synchronization");
        exit(1);
    }

    //Opening server
    puts("Opening server address\n");

    if (readAddrFromConfigFile("hostAddress.info", addrname)) {        
        puts("Failed reading address in configuration file");
        log(MERROR, "Failed openning server's address.");
        exit(1);
    }

    log(INFO,"Opening Server address");
    int serverFD = openAdress(addrname);
    if (serverFD < 0) {
        printf("Opening server address failed\n");
        log(MERROR,"Opening server address failed");
        exit(1);
	}

    puts("Server open. Listening...\n");

    clock_t begin=clock();
    while (1) {

        sem=sem_open(SEMNAME,0);

    	connection * con = readNewConnection(serverFD);
    	if(con!=NULL){
            log(INFO,"new client connected");
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
                printf("read:.%s.\n", smth);
                if(strcmp(smth, "q") == 0) {
                    puts("Closing server...");
                    return 0;
                }                
                while(getchar() !=  EOF);
            }
//            printf("\e[1;1H\e[2J");
//            drawChart();
//            begin=clock();
//            sem_wait(sem);
//            printf("STOCK %d\n",getStock(DBdata,"papa"));
	//            sem_post(sem);


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

    insertIntoTable(DBdata, "papa", 1000, 500);
    insertIntoTable(DBdata, "tomate", 1000, 500);
    insertIntoTable(DBdata, "pepino", 1000, 500);
    insertIntoTable(DBdata, "zanahoria", 1000, 500);
    insertIntoTable(DBdata, "remolacha", 1000, 500);
    insertIntoTable(DBdata, "zapallito", 1000, 500);
    insertIntoTable(DBdata, "zucchini", 1000, 500);
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
        log(MERROR,"ERROR EXECUTING SQLITE!");
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
    log(INFO,"Initializing UUID database...");
    for(i=0;i<n;i++){
        printf("\rIn progress [%.*s%*s] %.2f %%",(i+1)*30/n,"||||||||||||||||||||||||||||||||||||||||",30-(i+1)*30/n,"", (i*100.0)/n);
        fflush(stdout);
        UUIDadd(newUUID((uint64_t) random(),(uint64_t) random()));
    }
    log(INFO,"UUID insertion succesful");
    printf("\nUUID insertion succesful\n");
}
//This allocates and initialices the pricing information for later use
//when the price needs updating
int initialiceProducts(dataPrices_t * priceData ){
	int trackedProducts = 1;
	priceData = malloc(sizeof(productPriceData_t)*trackedProducts);
	//char* prodName, int initalPrice, int initialStock, int priceAt0Stock
	initialiceProduct(priceData->prods, "papa\0",3,30,10);
	// initialiceProduct(priceData->prods[1], "tomate\0",10,20,50);
}
int initialiceProduct(productPriceData_t * priceData, char* prodName,
											int initialPrice, int initialStock, int priceAt0Stock){

		strcpy(priceData->prodName,prodName);
		priceData->initialPrice = initialPrice;
		priceData->initialStock = initialStock;
		priceData->priceAt0Stock = priceAt0Stock;
		priceData->m = (initialPrice - priceAt0Stock)*1.0/initialStock;
}

int printProductPriceData(productPriceData_t * priceData){
	printf("Pricing data for %s\n", priceData->prodName);
	printf("initPrice: %d initStock: %d\n", priceData->initialPrice,priceData->initialStock);
	printf("m: %d priceAt0Stock: %d\n", priceData->m,priceData->priceAt0Stock);
}

int calculateProdPrice(productPriceData_t * priceData, int currentStock){
	int x = round(priceData->m * currentStock) + priceData->priceAt0Stock;
	if(x <= 0) return 1;
	return x;
}
