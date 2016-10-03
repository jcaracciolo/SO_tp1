#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <poll.h>
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

void freeConnection(connection * con) {
	free(con->inPath);
	free(con->outPath);
	free(con);
}

		/** 
	  * Create paths for two pipes (in and out) with the pid of the process.
	  * This way every pipe will have a unique name.
	  */
connection * connectToAddres(char * addr) {
	int pid = getpid(), i = 0, j = 0;
	connection * con = malloc(sizeof(connection));
	char hostName[MAX_BUF];

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

	while (addr[i++] != '/');
	while (addr[i] != '\0') {
		hostName[j++] = addr[i];
		i++;
	}
	hostName[j] = '\0';

	// send to addr info about the connection
	char fifoToConnect[MAX_BUF] = {0};
	strcpy(fifoToConnect, "/tmp/");
	strcat(fifoToConnect, hostName);

	if( access(fifoToConnect, F_OK ) == -1 ) {
    	puts("Pipe does not exist.");
        free(con->inPath);
        free(con->outPath);
    	exit(1);
	}

	int fdToConnect = open(fifoToConnect, O_WRONLY);
	char fifoPaths[MAX_BUF];


	strcpy(fifoPaths, con->outPath);
	strcat(fifoPaths, "\n");
	strcat(fifoPaths, con->inPath);

	write(fdToConnect, fifoPaths, strlen(fifoPaths)+1);
	
	con->outFD=open(con->outPath,O_WRONLY);
	con->inFD=open(con->inPath,O_RDONLY);

	return con;
}

int openAdress(char * addr) {
	char newFIFO[MAX_BUF] = {0}, hostName[MAX_BUF];
	int i = 0, j = 0;

	while (addr[i++] != '/');
	while (addr[i] != '\0') {
		hostName[j++] = addr[i];
		i++;
	}
	hostName[j] = '\0';

	strcpy(newFIFO, "/tmp/");
	strcat(newFIFO, hostName);
	mkfifo(newFIFO, 0666);
	return open(newFIFO, O_RDWR);	// O_RDWR beacause p
}

connection * readNewConnection(int fd) {
	struct pollfd poll_list[2];

    poll_list[0].fd = fd;
    poll_list[0].events = POLLIN;
    // poll checks if something was sent to fd
	int readSmth = poll(poll_list, (unsigned long) 1, 10);
	if(readSmth == 0) {
		return NULL;
	}
    connection * con = malloc(sizeof(connection));
    con->inPath = calloc(MAX_BUF, 1);
    con->outPath = calloc(MAX_BUF, 1);


    char buf[MAX_BUF]={0};

	int in=1,i;
	char aux=1;
	for(i=0;aux!='\0';i++){
		read(fd,&aux, 1);
		if(aux!='\n') {
			if (in) {
				con->inPath[i] = aux;
			} else {
				con->outPath[i] = aux;
			}
		}else{
			con->inPath[i] = '\0';
			in=0;
			i=-1;
		}
	}
	con->outPath[i] = '\0';

	openConnection(con);

	return con;
}

int sendBytes(connection * con, char * buffer, int len) {
	return write(con->outFD, buffer, len);
}

int receiveBytes(connection * con, char * buffer, int length) {
	return read(con->inFD, buffer, length);
}

void endConnection(connection * con) {	
	close(con->inFD);
	close(con->outFD);
}

void openConnection(connection* con){
	con->inFD=open(con->inPath,O_RDONLY);
	con->outFD=open(con->outPath,O_WRONLY);
}