#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "marsh.h"
#include "data_types.h"
#include "../Coms/coms.h"

//connects to an address ej: "192.168.13"
//returns NULL if it couldnt connect
connection * connect(char * addr){
    return connectToAddres(addr);
}

//Sends a null terminated string to the specified connection
int sendString(connection * con,char * str){
    sendBytes(con, str, strlen(str)+1);
    return 0;
}

//stores the string in buff, maximum len characters, including \0
//Non blocking
int receiveString(connection * con,char * buf, int lenght){
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
int receiveInt(connection * con, int * num){
    char  numHolder[sizeof(num)] = {0};
    receiveBytes(con, numHolder,sizeof(num));

    memcpy(num, numHolder, sizeof(num));
    return 0;
}
