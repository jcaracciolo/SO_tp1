#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "coms.h"

#define MAX_BUF 1024

int main(int argc, char ** argv) {
	printf("i am the child\n");
    // int fd;
    // char * myfifo = "/tmp/server_fifo";
    // char buf[MAX_BUF];

    //  /*open, read, and display the message from the FIFO */
    // fd = open(myfifo, O_RDONLY);
    // read(fd, buf, MAX_BUF);
    // printf("Received: %s\n", buf);
    // close(fd);

    return 0;
}
