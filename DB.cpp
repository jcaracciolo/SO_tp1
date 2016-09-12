#include <iostream>
#include <set>
#include <stdint.h>

typedef struct{
    uint64_t high;
    uint64_t low;
} UUID;


// {
//
//UUID newUUID(uint64_t high,uint64_t low);
//
//void DBadd(UUID id);
//
//void DBremove(UUID id);
//
//int DBcontains(UUID id);
//
//}

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

extern "C" void DBadd(UUID id){
    database.insert(id);
}

extern "C" void DBremove(UUID id){
    database.erase(id);
}

extern "C" int DBcontains(UUID id){
    return (int)(database.find(id) != database.end());
}
