#ifndef COMS_HNuevo 
#define COMS_H

typedef struct {
	char * path; // '\0' ended
} address;

typedef struct {
	char * path;
} connection;

int close(struct connection * con);
int read(struct connection * con, char * buffer, int length);
int write(struct connection * con, char buffer, int cant);
struct connection * accept(struct address * addr);
struct connection * connect(struct address * addr);

#endif
