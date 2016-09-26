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
		assist(con);
		printf("Child fork failed\n");
	}
}

void assist(connection* con) {
	sendBytes(con,"Mensaje de hijo a cliente", 26);
	char buf[300]={0};
	while(buf[0] == 0){
		receiveBytes(con,buf,300);
	}
	printf("%s\n", buf);
	exit(0);
}

int main(int argc, char *argv[]) {
	
	int serverFD = openAdress("12352.epord-linux-mint");
	if (serverFD < 0) {
		printf("Opening server address failed\n");
		exit(1);
	}
	connection * con = readNewConnection(serverFD);
	if(con!=NULL){
		createChild(con);
	} else {
		printf("Falied creating connection.\n");
	}
	return 0;
}