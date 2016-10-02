#ifndef MARSH_H
#define MARSH_H
#include "../DB/UUID_DataBase/data_types.h"
#include <limits.h>


//Amount of tries it will make before timing out
#define TIMEOUT_TRIES 60
//Amount of time in seconds it will wait until it tries again to connect
#define CONN_TRY_INT
#define BLOCK_SIZE 10
#define INITIALERROR 100

typedef struct adress_t address;

typedef struct connection_t connection;

typedef enum  {NOCONECTION=INT_MIN,INSUFPRODS,NOSTOCK,MAXUUIDS, MOREMONEY, LESSMONEY, INVALIDUUID,NOSUCHELEMENT,ACKNOWLEDGE,OK} conerrors_t;


//establishes a connection to the address
connection * connect(char * addr);

//ends all the transa
int disconnect(connection * con);

//Sends a null terminated string to the specified address
int sendString(connection * con,char * str);

//stores the string in buff, maximum len characters, including \0
int receiveString(connection * con,char * buf, int lenght);

//sends the num to the specified connection
int sendInt(connection * con, int num);

//recieves a number from the conection and saves it on num
int receiveInt(connection * con);

//sends the UUID array to the specified connection
int sendUUIDArray(connection * con, UUIDArray * array);

int sendSellTransaction( connection * con, char * prodName,int amount,
                         int minPrice, UUIDStock * stock, int * finalGain,int client);

//recieves a UUID array from the conection and stores it in array
//Asumes the array is empty.
UUIDStock* receiveUUIDArray(connection * con,int n);

//sned transType
int sendTransType(connection * con, transType_t transType);
//receive transType
transType_t receiveTransType(connection * con);

//sends ACK to the con
int sendACK(connection * con);
//receives an ACK, returns 0 if it received an ACK, -1 if not.
int receiveACK(connection * con);

//Receives the price from the bd
int getPriceFromDB(connection * con, char * prodName,int client);

//Receives the price from the bd
int getStockFromDB(connection * con, char * prodName, int client);

void printStock(UUIDStock * stock);


//Tries to buy with the determined price
int sendBuyTransaction( connection * con, char * prodName, int amount,
                        int maxPrice, UUIDStock * stock, int * finalCost,int client);

int getBuySellInfo(connection* con,int* client, char* prodName, int* amount,int* pay);


int getRequestedProduct(connection* con,int* client,char* prodName);

int completePurchase(connection* con,UUIDArray* data,int payed);


#endif
