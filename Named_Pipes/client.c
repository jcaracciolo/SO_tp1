#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "coms.h"

#define MAX_BUF 300

int main() {
	connection * con = connectToAddres("12352.epord-linux-mint");

	char buf[MAX_BUF]={0};
	while(buf[0] == 0){
		receiveBytes(con,buf,MAX_BUF);
	}

	printf("%s\n",buf);

	sendBytes(con, "Respuesta del cliente al hijo", 30);
}