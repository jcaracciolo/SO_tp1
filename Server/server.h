#ifndef SERVER_H
#define SERVER_H
#include "Coms/coms.h"
#include <semaphore.h>

typedef struct{
    int fdin;
    int fdout;
    sem_t* sem;
} dbdata_t;


void createChild(connection * con);
void assist(connection* con);

#endif