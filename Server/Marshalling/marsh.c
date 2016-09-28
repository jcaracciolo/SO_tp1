#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "marsh.h"
#include "../Coms/coms.h"
////ASADADASDASDSDSDAadasdasdsadasadssad

//connects to an address ej: "192.168.13"
//returns NULL if it couldnt connect
connection * connect(char * addr){
    return connectToAddres(addr);
}

//Sends a null terminated string to the specified connection
int sendString(connection * con,char * str){
    sendBytes(con, str, strlen(str)+1);
}

//stores the string in buff, maximum len characters, including \0
int receiveString(connection * con,char * buf, int lenght){
    receiveBytes(con, buf,lenght);
}
