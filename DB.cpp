#include <iostream>
#include <set>
#include "DB.h"


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

int main()
{

    database.insert(newUUID(1,2));
    database.insert(newUUID(1,2));
    database.insert(newUUID(1,2));

    UUID a=newUUID(1,2313213213);
    DBadd(a);

    std::cout << DBcontains(a)  <<"\n";
    std::cout << a.low  <<"\n";
}