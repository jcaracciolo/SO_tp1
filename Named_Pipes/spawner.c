#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "coms.h"

#define MAX_BUF 20

int main(int argc, char *argv[]) {
	int child1, child2;
	
	connection * con = connect(NULL);	
	printf("AAAAA\n");
    //con->inFD = open(myfifo, O_RDONLY);

    /* create the FIFO (named pipe) */
    //mkfifo(myfifo, 0666);



	int fd;
    char * server_fifo = "/tmp/server_fifo";
    char buf[MAX_BUF];
    mkfifo(server_fifo, 0666);




	if ((child1 = fork()) == 0) {
		// Child1
		char *args = 0;
		execv("./reader", args);
		printf("Shouldn't be printed\n");
	}

	if ((child2 = fork()) == 0) {
		// Child2
		char *args = 0;
		execv("./writer", args);
		printf("Shouldn't be printed\n");
	}

    /* open, read, and display the message from the FIFO */
    fd = open(server_fifo, O_RDONLY);	// 3 is the hardcoded fd of the server
    printf("READing...%i\n", fd);
    read(fd, buf, MAX_BUF);
    printf("READ!!%s\n", buf);



	waitpid(child1, NULL, 0);
	waitpid(child2, NULL, 0);

	unlink(con->inPath);
	unlink("/tmp/server_pipe");
	return 0;
}