#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "marsh.h"
#include "../Coms/coms.h"



void printStock(UUIDStock * stock);
void addUUIDsToStock(UUIDStock * stock, UUIDStock * newUUIDS);
void printArray(UUIDArray * stock);

// connection * connect(char * addr){
//   return connectToAddres(addr);
// }

int countStock(UUIDStock * stock) {
  return stock->last;
}

int disconnect(connection * con){
  sendTransType(con, CLOSE);
  endConnection(con);
}

//Sends a null terminated string to the specified connection
int sendString(connection * con,char * str){
    sendBytes(con, str, strlen(str)+1);
    return 0;
}

//stores the string in buff, maximum len characters, including \0
//busy blockings
int receiveString(connection * con,char * buf, int lenght){
    if(lenght > 0) buf[0] = 0;
    else return -1;
      receiveBytes(con, buf,lenght);
    return 0;
}

//sends the num to the specified connection
int sendInt(connection * con, int num){
    char  numHolder[sizeof(num)];
    memcpy(numHolder, &num, sizeof(num));
    sendBytes(con, &num, sizeof(num));
    return 0;
}

//recieves a number from the conection and saves it on num
//Its blocking
int receiveInt(connection * con){
    char numHolder[sizeof(int)] = {0};
    receiveBytes(con, numHolder,sizeof(int));
    return *(int*)(numHolder);
}

int sendUUIDArray(connection * con, UUIDArray * array){
  sendBytes(con,(char*)array->uuids, sizeof(UUID)*array->size);
  return 0;
}

UUIDStock* receiveUUIDArray(connection * con, int n){
    UUIDStock* ans=malloc(sizeof(UUIDStock));
    ans->uuids=malloc(sizeof(UUID)*n);

    // printf("recieved UUIDS:\n");
    int read=receiveBytes(con,(char*)ans->uuids,sizeof(UUID)*n);
    ans->size=n;
    ans->last=read/sizeof(UUID);
    int i;

    printf("I am receing this UUIDS\n");
    printStock(ans);


    return ans;
}

int sendTransType(connection * con, transType_t transType){
  sendInt(con, transType);
}

transType_t receiveTransType(connection * con){
  return (transType_t) receiveInt(con);
}

int sendACK(connection * con){
  sendTransType(con, ACKNOWLEDGE);
}

int receiveACK(connection * con){
  if(receiveInt(con) != ACKNOWLEDGE)
    return 0;
  else return 1;
}

int getPriceFromDB(connection * con, char * prodName,int client){
  sendTransType(con, PRICE);
    if(!receiveACK(con)) return NOCONECTION;
  sendInt(con,client);
    if(!receiveACK(con)) return NOCONECTION;
  sendString(con, prodName);
  return receiveInt(con);
}

int getStockFromDB(connection * con, char * prodName,int client){
  sendTransType(con, STOCK);
    if(!receiveACK(con)) return NOCONECTION;
  sendInt(con,client);
    if(!receiveACK(con)) return NOCONECTION;
  sendString(con, prodName);
  return receiveInt(con);
}

int isProdInDB(connection * con, char * prodName,int client) {
  sendTransType(con, EXISTS);
    if(!receiveACK(con)) return NOCONECTION;
  sendInt(con,client);
    if(!receiveACK(con)) return NOCONECTION;
  sendString(con, prodName);
  return receiveInt(con);
}

int getRequestedProduct(connection* con,int* client,char* prodName){
    sendACK(con);
    *client=receiveInt(con);
    sendACK(con);
    receiveString(con, prodName,MAX_PROD_NAME_LENGHT);
    return 0;
}

readAddrFromConfigFile(char * path, char * addr) {
    FILE * addrSource = fopen(path, "r");
    if (addrSource == NULL) {
      return -1;
    }
    char separators[] = {':', '/'};
    int i = 0, j = 0, c;
    c = fgetc(addrSource);
    while(c != EOF) {
        if (c == '\n') {
            addr[i++] = separators[j++];
        } else {
            addr[i++] = c;
        }
        c = fgetc(addrSource);
    }
    addr[i] = '\0';

    fclose(addrSource);

    if (j != 2) {
      return -1;
    }
    return 0;
}

//This sends a buy transaction to the conection. Returns 1 if succesful
// and 0 if not succesful. This is used exclusively by the client.

int sendSellTransaction( connection * con, char * prodName,int amount,
                        int minPrice, UUIDStock * stock, int * finalGain,int client){

    int r;

    if(stock->last<amount){
        return INSUFPRODS;
    }

    sendTransType(con,SELL);
    if(!receiveACK(con)) return NOCONECTION;

    sendInt(con,client);
    if(!receiveACK(con)) return NOCONECTION;

    //Receive prodname and send ack
    sendInt(con,amount);
    if(!receiveACK(con)) return NOCONECTION;

    //Send amount of product to buy
    sendInt(con,minPrice);
    if(!receiveACK(con)) return NOCONECTION;

    sendString(con,prodName);



    UUIDArray tosell;
    int i,off =  stock->last - amount;
    for (i =0; i < amount; i++) {
        tosell.uuids[i].high = stock->uuids[off+i].high;
        tosell.uuids[i].low = stock->uuids[off+i].low;
    }
    // memccpy(tosell.uuids,(void*)&(stock->uuids[stock->last-amount]),amount,sizeof(UUID));
    tosell.size=amount;

    if((r=receiveInt(con))!=ACKNOWLEDGE)
        return (r==0)?NOCONECTION:r;

    sendUUIDArray(con,&tosell);

    if((r=receiveInt(con)) == SUCCESS){

      // stock->uuids = realloc(stock->uuids,(stock->last - amount)*sizeof(UUID));
      stock->last-=amount;
      stock->size;
        sendACK(con);
        *finalGain=receiveInt(con);
        return 0;
    } else {
      printf("Something whrong:\n");
        return r;
        // printf("The transaction didnt go through:\n");
    }
}


int getBuySellInfo(connection* con,int *client, char *prodName, int *amount, int *pay){
    sendACK(con);
    *client=receiveInt(con);
    sendACK(con);

    //receive amount of product to buy
    *amount=receiveInt(con);
    sendACK(con);

    //receive max price the client is willing to pay
    *pay=receiveInt(con);
    sendACK(con);

    //Receive prodname and send ack
    receiveString(con, prodName,MAX_PROD_NAME_LENGHT);




    return 0;
}

int sendBuyTransaction( connection * con, char * prodName,int amount,
                        int maxPrice, UUIDStock * stock, int * finalCost,int client){

    int r;
  sendTransType(con,BUY);
    if(!receiveACK(con)) return NOCONECTION;

  sendInt(con,client);
    if(!receiveACK(con)) return NOCONECTION;

  //Receive prodname and send ack
    sendInt(con,amount);
    if(!receiveACK(con)) return NOCONECTION;

    //Send amount of product to buy

    sendInt(con,maxPrice);
    if(!receiveACK(con)) return NOCONECTION;

    sendString(con,prodName);

    if((r=receiveTransType(con)) == SUCCESS){
        printf("revieving ok");

      sendACK(con);
      UUIDStock *ans=receiveUUIDArray(con,amount);

      sendACK(con);
      *finalCost=receiveInt(con);

      addUUIDsToStock(stock,ans);

    // printf("%d after local UUIDS:\n",stock->last);
    // printStock(stock);
    //receive total amount payed
    // printf("sent ack\n");
    // int total = receiveInt(con);
    // receiveACK(con);
    // printf("Total i payed: %d\n",*finalCost  );

    // printf("The transaction went through:\n");
    sendACK(con);
    //This means the transaction went through

    return 0;


  } else {
        return r;
    // printf("The transaction didnt go through:\n");
  }
  //This means the transaction didnt go through
  return 0;
}
void printStock(UUIDStock * stock){
  int i;
  for(i=0;i<stock->last;i++){
    printf("%ld - %ld\n",stock->uuids[i].high,stock->uuids[i].low);
  }
}
void printArray(UUIDArray * stock){
  int i;
  for(i=0;i<stock->size;i++){
    printf("%ld - %ld\n",stock->uuids[i].high,stock->uuids[i].low);
  }
}

void addUUIDsToStock(UUIDStock * stock, UUIDStock * newUUIDS){
  //DEBUGGING PRINTF AND LINES THAT MIGHT BE USEFULL SOMEDAY
  // printf("last: %d, size: %d\n",stock->last,stock->size);
  // printf("last: %d, size: %d\n",newUUIDS->last,newUUIDS->size);
  //
  // // printf("what i see in stock:\n");
  // //
  // // printf("waht i see in new:\n");
  // // printStock(newUUIDS);
  // printf("stock %p, skip %d",stock->uuids,(newUUIDS->last * sizeof(UUID)));
  // printf("%ld \n",newUUIDS->uuids[0].high);
  // // memcpy( stock->uuids + (stock->last * sizeof(UUID)),
  // //         newUUIDS->uuids, newUUIDS->last * sizeof(UUID));
  // printf("last: %d, size: %d\n",stock->last,stock->size);

  if(stock->size < newUUIDS->last + stock->last){
    stock->uuids = realloc( stock->uuids,
                            (newUUIDS->last + stock->last + 1 ) * sizeof(UUID));
  }
  // printStock(stock);
  int off = stock->last;
  size_t i;
  for (i = 0; i < newUUIDS->last; i++) {
      stock->uuids[i + off].high = newUUIDS->uuids[i].high;
      stock->uuids[i + off].low = newUUIDS->uuids[i].low;
  }

  stock->size = newUUIDS->last + stock->last + 1;
  stock->last = stock->last + newUUIDS->last ;

  free(newUUIDS->uuids);
  free(newUUIDS);
}

int completePurchase(connection* con,UUIDArray* data,int payed){
    sendTransType(con,SUCCESS);
    if(!receiveACK(con)) return NOCONECTION;
    sendUUIDArray(con,data);
    if(!receiveACK(con)) return NOCONECTION;
    sendInt(con,payed);

}
