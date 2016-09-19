#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include "coms.h"

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

	printf("%s\n", con->inPath);
	printf("%s\n", con->outPath);

	//mkfifo("/tmp/myfifo", 0666);
	printf("AAAAARRR\n");
	con->inFD = open(con->inPath, O_RDONLY | O_NONBLOCK);
	con->outFD = open(con->outPath, O_RDWR);
	int serverFD = open("/tmp/serverFIFO", O_WRONLY);
	write(serverFD, con->outPath, strlen(con->outPath));
	write(serverFD, con->inPath, strlen(con->inPath)+1);

	return con;
}

int openAdress(char * ip) {
	char * serverFIFO = "/tmp/serverFIFO";
	mkfifo(serverFIFO, 0666);
	return open(serverFIFO, O_RDONLY | O_NONBLOCK);
}