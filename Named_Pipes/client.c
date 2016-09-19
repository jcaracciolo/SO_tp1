#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include "coms.h"

int main() {
	connection * con = connect("190.server.com");
	openConnection(con);
	char buf[300]={0};

	//send(con,"PA EL SERVERE",14);
	while(buf[0]==0){
	receive(con,buf,300);
	}

	printf("%s\n",buf);
	while(1) {
		int a = 0;
		a++;
	}
}