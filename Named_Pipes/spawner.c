#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
	int child1, child2;

    char * myfifo = "/tmp/myfifo";

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);

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

	waitpid(child1);
	waitpid(child2);

	unlink(myfifo);

	return 0;
}
