#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "coms.h"
#include "server.h"

#define MAX_BUF 300

void createChild(connection * con) {
	int childPID;

	if ((childPID = fork()) == 0) {
		// Child
		openConnection(con);
		assist(con);
		printf("Child fork failed\n");
	}
}

void assist(connection* con) {
	send(con,"Mensaje de hijo a cliente", 26);
	char buf[300]={0};
	while(buf[0] == 0){
		receive(con,buf,300);
	}
	printf("%s\n", buf);
	exit(0);
}

int main(int argc, char *argv[]) {
	
	int serverFD = openAdress("190.server.com");
	if (serverFD == -1) {
		printf("Opening server address failed/n");
		exit(1);
	}
	connection * con = readNewConnection(serverFD);
	if(con!=NULL){
		createChild(con);
	}

	return 0;
}