#ifndef COMS_HNuevo 
#define COMS_H

typedef struct adress_t {
	char * path; // '\0' ended
} address;

typedef struct connection_t {
	char * inPath;
	char * outPath;
	int inFD;
	int outFD;
} connection;

//int close(struct connection * con);
//int read(struct connection * con, char * buffer, int length);
//int write(struct connection * con, char buffer, int cant);
//connection * accept(struct address * addr);
connection * connect(address * addr);

#endif
