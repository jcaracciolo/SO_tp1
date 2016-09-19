#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "coms.h"

#define MAX_BUF 300

void createChild(char * pipe ) {
	int child1;

	if ((child1 = fork()) == 0) {
		// Child1
		char *args = 0;
		execv("./reader", args);
		printf("Reader fork failed\n");
	}

}

int main(int argc, char *argv[]) {
	
	int serverFD = openAdress("190.server.com");
	if (serverFD == -1) {
		printf("Opening adress failed/n");
		exit(1);
	}

	char * buf = malloc(MAX_BUF);
	buf[0]=0;
	while (buf[0]==0) {
		read(serverFD, buf, MAX_BUF);
		printf("%s\n", buf);
	}buf[0]=0;
	while (buf[0]==0) {
		read(serverFD, buf, MAX_BUF);
		printf("%s\n", buf);
	}

	return 0;
}