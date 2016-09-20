#ifndef COMS_HNuevo 
#define COMS_H

typedef struct adress_t address;

typedef struct connection_t connection;

//int close(struct connection * con);
int receive(connection * con, char * buffer, int length);
int send(connection * con, char * buffer, int cant);
//connection * accept(struct address * addr);
connection * connect(char * addr);
int openAdress(char * ip);
connection * readNewConnection(int serverFD);
void openConnection(connection* con);

#endif
