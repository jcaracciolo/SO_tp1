#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../Server/Coms/coms.h"

#define MAX_BUF 300

int main() {
	char hostname[250];
	char buffer[250];
	gethostname(hostname,250);
	strcpy(buffer,"12352.");
	strcat(buffer,hostname);
	connection * con = connectToAddres(buffer);

	char buf[MAX_BUF]={0};
	while(buf[0] == 0){
		receiveBytes(con,buf,MAX_BUF);
	}

	printf("%s\n",buf);

	sendBytes(con, "Respuesta del cliente al hijo", 30);
}
