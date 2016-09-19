#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include "coms.h"

#define MAX_BUF 300

struct connection_t {
	char * inPath;
	char * outPath;
	int inFD;
	int outFD;
};

int countDigits(int n) {
	int count = 0;
	while(n!=0) {
        n /= 10;
        ++count;
    }
    return count;
}

connection * connect(address * addr) {
	umask(0);
	int pid = getpid();
	connection * con = malloc(sizeof(connection));

	if ((con->inPath = malloc(14 + countDigits(pid))) == NULL) {
		printf("Cannot create pipe. Lack of memory.\n");
		return 0;
	}
	sprintf(con->inPath, "/tmp/%i_fifo_in", pid);
	if (mkfifo(con->inPath, 0666) == -1) {
		printf("Couldn't create FIFO.\n");
		return 0;
	}

	if ((con->outPath = malloc(15 + countDigits(pid))) == NULL) {
		printf("Cannot create pipe. Lack of memory.\n");
		return 0;
	}
	sprintf(con->outPath, "/tmp/%i_fifo_out", pid);
	if (mkfifo(con->outPath, 0666) == -1) {
		printf("Couldn't create FIFO.\n");
		return 0;
	}

	// con->inFD = open(con->inPath, O_RDONLY | O_NONBLOCK);
	// con->outFD = open(con->outPath, O_RDWR);

	// send to server info about the connection
	int serverFD = open("/tmp/serverFIFO", O_WRONLY);
	write(serverFD, con->outPath, strlen(con->outPath)+1);
	write(serverFD, con->inPath, strlen(con->inPath)+1);

	return con;
}

int openAdress(char * ip) {
	if (strcmp("190.server.com", ip) == 0) {
		char * serverFIFO = "/tmp/serverFIFO";
		mkfifo(serverFIFO, 0666);
		return open(serverFIFO, O_RDONLY | O_NONBLOCK);
	} else {
		printf("IP incorrect\n");
		return -1;
	}
}

connection * readFromServerAdress(int serverFD) {
	connection * com = malloc(sizeof(connection));
	com->inPath = malloc(MAX_BUF);
	com->outPath = malloc(MAX_BUF);
	
	char buf[MAX_BUF];
	buf[0] = 0;
	while (buf[0] == 0) {
		read(serverFD, buf, MAX_BUF);
	}
	strcpy(com->inPath, buf);
	printf("%s\n", com->inPath);

	buf[0] = 0;
	while (buf[0] == 0) {
		read(serverFD, buf, MAX_BUF);
	}
	strcpy(com->outPath, buf);
	printf("%s\n", com->outPath);

	// com->inFD = open(com->inPath, O_RDONLY | O_NONBLOCK);
	// com->outFD = open(com->outPath, O_RDWR);

	return com;
}

void openConnection(connection* con){
	con->inFD=open(con->inPath,O_RDONLY | O_NONBLOCK);
	printf("begFunct\n");
	con->outFD=open(con->outPath,O_RDWR);
	printf("endiFunct\n");
}

int send(connection * con, char * buffer, int len) {
	printf("sending to...%s\n",con->outPath);
	write(con->outFD, buffer, len);
	printf("written\n");
	return 0;
}

int receive(connection * con, char * buffer, int length) {
	printf("receiving from..%s\n",con->inPath);
	read(con->inFD, buffer, length);
	return 0;
}