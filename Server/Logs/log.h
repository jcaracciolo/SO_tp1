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
#define MAX_MSG 300
typedef struct{
    long mtype;
    char message[MAX_MSG];
}msgbuf_t;

typedef enum  {INFO=1,WARNING, MERROR} msg_t;



#endif //SO_TP1_LOG_H
