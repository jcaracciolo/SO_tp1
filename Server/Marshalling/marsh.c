#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "marsh.h"
#include "../Coms/coms.h"

connection * connect(char * addr){
  return connectToAddres(addr);
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

UUIDStock* receiveUUIDArray(connection * con, int n,int* cost){
    UUIDStock* ans=malloc(sizeof(UUIDStock));
    ans->uuids=malloc(sizeof(UUID)*n);

    int read=receiveBytes(con,(char*)ans->uuids,sizeof(UUID)*n);
    ans->size=n;
    ans->last=read/sizeof(UUID);

    sendInt(con,ACKNOWLEDGE);
    *cost=receiveInt(con);

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

int getPriceFromDB(connection * con, char * prodName){
  sendTransType(con, PRICE);
  receiveACK(con);
  sendString(con, prodName);
  return receiveInt(con);
}

int getStockFromDB(connection * con, char * prodName){
  sendTransType(con, STOCK);
  receiveACK(con);
  sendString(con, prodName);
  return receiveInt(con);
}

int sendBuyTransaction( connection * con, char * prodName,int amount,
                        int maxPrice, UUIDStock * stock){

  sendTransType(con,BUY);
  receiveACK(con);

  //Receive prodname and send ack
  sendString(con, "papa\0");
  receiveACK(con);

  //Send amount of product to buy
  sendInt(con,amount);
  receiveACK(con);

  sendInt(con,maxPrice);

  if(receiveTransType(con) == OK){
    printf("The trying to send uuids:\n");
    int cost;
    sendACK(con);
    UUIDStock *ans=receiveUUIDArray(con,amount,&cost);

    printf("recieved UUIDS:\n");
    for(int i=0;i<ans->last;i++){
      printf("%ld - %ld\n",ans->uuids[i].high,ans->uuids[i].low);
    }
    printf("previous local UUIDS:\n");
    for(int i=0;i<stock->last;i++){
      printf("%ld - %ld\n",stock->uuids[i].high,stock->uuids[i].low);
    }

    if(stock->size < amount + stock->last ){
      stock->uuids = realloc( stock->uuids,  stock->size * 2 * sizeof(UUID));
    }
    memcpy(stock->uuids + stock->last * sizeof(UUID), ans->uuids, amount * sizeof(UUID));
    stock->size = stock->size * 2;
    stock->last = stock->last + ans->last ;

    printf("%d after local UUIDS:\n",stock->last);
    for(int i=0;i<stock->last;i++){
      printf("%ld - %ld\n",stock->uuids[i].high,stock->uuids[i].low);
    }
    sendACK(con);
    //receive total amount payed
    printf("sent ack\n");
    int total = receiveInt(con);
    printf("Total i payed: %d\n",total);

    sendACK(con);
    printf("The transaction went through:\n");
  } else {
    printf("The transaction didnt go through:\n");
  }


  return 1;
}
