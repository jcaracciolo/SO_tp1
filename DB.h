//
// Created by juanfra on 12/09/16.
//
#include <ctype.h>

#ifndef SO_TP1_DB_H
#define SO_TP1_DB_H

typedef struct{
    uint64_t  high;
    uint64_t  low;
} UUID;

extern "C" UUID newUUID(uint64_t high,uint64_t low);

extern "C" void DBadd(UUID id);

extern "C" void DBremove(UUID id);

extern "C" int DBcontains(UUID id);

#endif //SO_TP1_DB_H
