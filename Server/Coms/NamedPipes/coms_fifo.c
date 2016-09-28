#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "../coms.h"

#define MAX_BUF 300


struct adress_t {
	char * path; // '\0' ended
};

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

connection * connectToAddres(char * addr) {

	/** 
	  * Create paths for two pipes (in and out) with the pid of the process.
	  * This way every pipe will have a unique name.
	  */
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

	con->inFD=open(con->inPath,O_RDONLY | O_NONBLOCK);
	con->outFD=open(con->outPath,O_RDWR);

	// send to addr info about the connection	
	char fifoToConnect[MAX_BUF] = {0};
	strcpy(fifoToConnect, "/tmp/");
	strcat(fifoToConnect, addr);

	int fdToConnect = open(fifoToConnect, O_WRONLY);
	write(fdToConnect, con->outPath, strlen(con->outPath)+1);
	write(fdToConnect, con->inPath, strlen(con->inPath)+1);

	return con;
}

int openAdress(char * addr) {
	char newFIFO[MAX_BUF] = {0};
	strcpy(newFIFO, "/tmp/");
	strcat(newFIFO, addr);
	mkfifo(newFIFO, 0666);
	return open(newFIFO, O_RDONLY | O_NONBLOCK);
}

connection * readNewConnection(int fd) {
	connection * con = malloc(sizeof(connection));
	con->inPath = malloc(MAX_BUF);
	con->outPath = malloc(MAX_BUF);
	
	char buf[MAX_BUF];
	buf[0] = 0;
	while (buf[0] == 0) {
		read(fd, buf, MAX_BUF);
	}
	strcpy(con->inPath, buf);

	buf[0] = 0;
	while (buf[0] == 0) {
		read(fd, buf, MAX_BUF);
	}
	strcpy(con->outPath, buf);

	con->inFD=open(con->inPath,O_RDONLY | O_NONBLOCK);
	con->outFD=open(con->outPath,O_RDWR);

	return con;
}

int sendBytes(connection * con, char * buffer, int len) {
	write(con->outFD, buffer, len);
	return 0;
}

int receiveBytes(connection * con, char * buffer, int length) {
	read(con->inFD, buffer, length);
	return 0;
}