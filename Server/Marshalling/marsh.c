#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "marsh.h"
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
int receiveInt(connection * con, int * num){
    char  numHolder[sizeof(num)] = {0};
    receiveBytes(con, numHolder,sizeof(num));
    memcpy(num, numHolder, sizeof(num));
    return 0;
}

int sendUUIDArray(connection * con, UUIDArray * array){
  char  numHolder[sizeof(UUIDArray)];
  memcpy(numHolder, array, sizeof(UUIDArray));
  sendBytes(con, numHolder, sizeof(UUIDArray));
  return 0;

}

int receiveUUIDArray(connection * con, UUIDArray * array){
    char  numHolder[sizeof(UUIDArray)] = {0};
    receiveBytes(con, numHolder,sizeof(UUIDArray));
    memcpy(array, numHolder, sizeof(UUIDArray));
    return 0;
}
