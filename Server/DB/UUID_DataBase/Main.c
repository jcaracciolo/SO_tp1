//
// Created by juanfra on 12/09/16.
//

#include <stdio.h>
#include "DB.h"
#include <stdint.h>

int main(){

    UUID a = newUUID(128,12);
    DBadd(a);
    printf("%d\n",DBcontains(a));

    return 0;

}
