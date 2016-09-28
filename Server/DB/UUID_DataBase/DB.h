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

void UUIDadd(UUID id);

void UUIDremove(UUID id);

int UUIDcontains(UUID id);

UUID getRandomUUID();



#endif //SO_TP1_DB_H
