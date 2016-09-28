#include <iostream>
#include <set>
#include <stdint.h>
#include <stdlib.h>

typedef struct{
    uint64_t high;
    uint64_t low;
} UUID;

bool operator<(const UUID &a,const UUID &b){
    return (a.high==b.high)? a.low<b.low : a.high < b.high ;
}

static std::set<UUID> database;

extern "C" UUID newUUID(uint64_t high,uint64_t low){
    UUID ans;
    ans.high=high;
    ans.low=low;
    return ans;
}

extern "C" void UUIDadd(UUID id){
    database.insert(id);
}

extern "C" void UUIDremove(UUID id){
    database.erase(id);
}

extern "C" int UUIDcontains(UUID id){
    return (int)(database.find(id) != database.end());
}

extern "C" UUID getRandomUUID(){
    return *(database.lower_bound(newUUID((uint64_t)random(),(uint64_t)random())));
}