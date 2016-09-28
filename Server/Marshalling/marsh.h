#ifndef MARSH_H
#define MARSH_H

//Amount of tries it will make before timing out
#define TIMEOUT_TRIES 60
//Amount of time in seconds it will wait until it tries again to connect
#define CONN_TRY_INTERVAL 1

typedef struct adress_t address;

typedef struct connection_t connection;

//Sends a null terminated string to the specified address
int sendString(connection * con,char * str);

//connects to an address ej: "192.168.13"
connection * connect(char * addr);

int receiveString(connection * con,char * buf, int lenght);

#endif
