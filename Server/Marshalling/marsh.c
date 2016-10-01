#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "marsh.h"
#include "../Coms/coms.h"
void printStock(UUIDStock * stock);
void addUUIDsToStock(UUIDStock * stock, UUIDStock * newUUIDS);

// connection * connect(char * addr){
//   return connectToAddres(addr);
// }

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
    sendBytes(con, numHolder, sizeof(num));
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
  if(receiveTransType(con) != ACKNOWLEDGE)
    return -1;
  else return 0;
}

int getPriceFromDB(connection * con, char * prodName,int client){
  sendTransType(con, PRICE);
  receiveACK(con);
  sendInt(con,client);
  receiveACK(con);
  sendString(con, prodName);
  return receiveInt(con);
}

int getStockFromDB(connection * con, char * prodName,int client){
  sendTransType(con, STOCK);
  receiveACK(con);
  sendInt(con,client);
  receiveACK(con);
  sendString(con, prodName);
  return receiveInt(con);
}

//This sends a buy transaction to the conection. Returns 1 if succesful
// and 0 if not succesful. This is used exclusively by the client.

int sendSellTransaction( connection * con, char * prodName,int amount,
                        int minPrice, UUIDStock * stock, int * finalGain,int client){

    if(stock->last<amount){
        printf("not enough to sell");
        return -1;
    }

    sendTransType(con,SELL);
    receiveACK(con);

    sendInt(con,client);
    receiveACK(con);

    //Receive prodname and send ack
    sendString(con,prodName);
    receiveACK(con);

    //Send amount of product to buy
    sendInt(con,amount);
    receiveACK(con);

    sendInt(con,minPrice);

    UUIDArray tosell;
    memccpy(tosell.uuids,(void*)&(stock->uuids[stock->last-amount]),amount,sizeof(UUID));
    tosell.size=amount;

    receiveACK(con);

    realloc(stock->uuids,(stock->last - amount)*sizeof(UUID));
    stock->last-=amount;
    stock->size=stock->last - amount;
    sendUUIDArray(con,&tosell);


    receiveACK(con);


    if(receiveTransType(con) == OK){

        // printf("The trying to send uuids:\n");
        sendACK(con);

        *finalGain=receiveInt(con);
        sendACK(con);
        return 1;
    } else {
        // printf("The transaction didnt go through:\n");
    }
    sendACK(con);
    //This means the transaction didnt go through
    return 0;
}


int sendBuyTransaction( connection * con, char * prodName,int amount,
                        int maxPrice, UUIDStock * stock, int * finalCost,int client){

  sendTransType(con,BUY);
  receiveACK(con);

  sendInt(con,client);
  receiveACK(con);

  //Receive prodname and send ack
  sendString(con,prodName);
  receiveACK(con);

    //Send amount of product to buy
    sendInt(con,amount);
    receiveACK(con);

    sendInt(con,maxPrice);

    if(receiveTransType(con) == OK){

        // printf("The trying to send uuids:\n");
        sendACK(con);
    UUIDStock *ans=receiveUUIDArray(con,amount);

    sendACK(con);
    *finalCost=receiveInt(con);

    // printf("recieved UUIDS:\n");
    // printStock(ans);

    // printf("previous local UUIDS:\n");
    // printStock(stock);

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
    return 1;
  } else {
    // printf("The transaction didnt go through:\n");
  }
  sendACK(con);
  //This means the transaction didnt go through
  return 0;
}
void printStock(UUIDStock * stock){
  int i;
  for(i=0;i<stock->last;i++){
    printf("%ld - %ld\n",stock->uuids[i].high,stock->uuids[i].low);
  }
}

void addUUIDsToStock(UUIDStock * stock, UUIDStock * newUUIDS){
  //DEBUGGING PRINTF AND LINES THAT MIGHT BE USEFULL SOMEDAY
  // printf("last: %d, size: %d\n",stock->last,stock->size);
  // printf("last: %d, size: %d\n",newUUIDS->last,newUUIDS->size);
  //
  // // printf("what i see in stock:\n");
  // // printStock(stock);
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
  int off = stock->last;
  size_t i;
  for (i = 0; i < newUUIDS->last; i++) {
      stock->uuids[i + off].high = newUUIDS->uuids[i].high;
      stock->uuids[i + off].low = newUUIDS->uuids[i].low;
  }

  stock->size = newUUIDS->last + stock->last + 1;
  stock->last = stock->last + newUUIDS->last ;

  free(newUUIDS);
}
