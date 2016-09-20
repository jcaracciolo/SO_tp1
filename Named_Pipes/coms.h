#ifndef COMS_HNuevo 
#define COMS_H

typedef struct adress_t {
	char * path; // '\0' ended
} address;

typedef struct communication_t {
	char * inPath;
	char * outPath;
} communication;

typedef struct connection_t connection;

//int close(struct connection * con);
int receive(connection * con, char * buffer, int length);
int send(connection * con, char * buffer, int cant);
//connection * accept(struct address * addr);
connection * connect(char * addr);
int openAdress(char * ip);
connection * readFromServerAdress(int serverFD);
void openConnection(connection* con);

#endif
