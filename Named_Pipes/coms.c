#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "coms.h"

int countDigits(int n) {
	int count = 0;
	while(n!=0) {
        n /= 10;
        ++count;
    }
    return count;
}

connection * connect(address * addr) {
	int pid = getpid();
	connection * con = malloc(sizeof(connection));

	if ((con->outPath = malloc(15 + countDigits(pid))) == NULL) {
		printf("Cannot create pipe. Lack of memory.\n");
		return 0;
	}
	sprintf(con->outPath, "/tmp/%i_fifo_out", pid);
	if (mkfifo(con->outPath, 0666) == -1) {
		printf("Couldn't create FIFO.\n");
		return 0;
	}

	if ((con->inPath = malloc(14 + countDigits(pid))) == NULL) {
		printf("Cannot create pipe. Lack of memory.\n");
		return 0;
	}
	sprintf(con->inPath, "/tmp/%i_fifo_in", pid);
	if (mkfifo(con->inPath, 0666) == -1) {
		printf("Couldn't create FIFO.\n");
		return 0;
	}

	//con->inFD = open(con->inFD, O_RDONLY);
	//con->outFD = open(con->outFD, O_WRONLY);

	return con;
}