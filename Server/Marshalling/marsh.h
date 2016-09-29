#ifndef MARSH_H
#define MARSH_H
#include "../DB/UUID_DataBase/data_types.h"

//Amount of tries it will make before timing out
#define TIMEOUT_TRIES 60
//Amount of time in seconds it will wait until it tries again to connect
#define CONN_TRY_INT

typedef struct adress_t address;

typedef struct connection_t connection;

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

//recieves a UUID array from the conection and stores it in array
//Asumes the array is empty
UUIDStock* receiveUUIDArray(connection * con,int n,int* cost);


#endif
