//
// Created by juanfra on 12/09/16.
//
#include <stdint.h>

#ifndef SO_TP1_DB_H
#define SO_TP1_DB_H

typedef struct{
    uint64_t  high;
    uint64_t  low;
} UUID;

UUID newUUID(uint64_t high,uint64_t low);

void DBadd(UUID id);

void DBremove(UUID id);

int DBcontains(UUID id);



#endif //SO_TP1_DB_H
