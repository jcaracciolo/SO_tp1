#ifndef COMS_H
#define COMS_H

#define END_OF_CONNECTION "KILLMEPLZ"

typedef struct adress_t address;

typedef struct connection_t connection;

//int close(struct connection * con);
int receiveBytes(connection * con, char * buffer, int length);
int sendBytes(connection * con, char * buffer, int cant);
//connection * accept(struct address * addr);
connection * connectToAddres(char * addr);
int openAdress(char * ip);
connection * readNewConnection(int serverFD);
void endConnection(connection * con);
void openConnection(connection* con);

#endif