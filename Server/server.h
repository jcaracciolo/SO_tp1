#ifndef SERVER_H
#define SERVER_H
#include "Coms/coms.h"

void createChild(connection * con);
void assist(connection* con);
void initializeUUID(unsigned int n);

#endif
