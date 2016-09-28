#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../Server/Marshalling/marsh.h"

#define MAX_BUF 300


int main() {
	char hostname[250];
	char buffer[250];

	gethostname(hostname,250);
	strcpy(buffer,"12352.");
	strcat(buffer,hostname);
	connection * con = connect(buffer);

	puts("Price of papa?");
	sendBytes(con, "get_price_of papa", 19);

	char buf[MAX_BUF]={0};
	while(buf[0] == 0){
		receiveString(con,buf,MAX_BUF);
	}
	printf("%s\n", buf);

	puts("Stock of papa?");
	sendBytes(con, "get_stock_of papa", 19);

	buf[0] = 0;
	while(buf[0] == 0){
		receiveBytes(con,buf,MAX_BUF);
	}

	sendBytes(con, END_OF_CONNECTION, 19);

	printf("%s\n",buf);

	sendString(con, "String mandado por cliente\0");
}
