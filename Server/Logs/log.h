//
// Created by juanfra on 30/09/16.
//

#ifndef SO_TP1_LOG_H
#define SO_TP1_LOG_H

#include <sys/msg.h>
#include <stddef.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    long mtype;
    char message[300];
}msgbuf_t;

typedef enum  {INFO=1,WARNING, MERROR} msg_t;



#endif //SO_TP1_LOG_H
