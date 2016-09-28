#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../Server/Marshalling/marsh.h"

#define END_OF_CONNECTION "KILLMEPLZ"

#define MAX_BUF 300


int main() {
	char hostname[250];
	char buffer[250];
	char temp[250];
	int asd;
	gethostname(hostname,250);
	strcpy(buffer,"12352.");
	strcat(buffer,hostname);
	connection * con = connect(buffer);

	puts("Price of papa?");
	sendString(con, "get_price_of papa");
	receiveInt(con, &asd );
	sprintf(temp, "the numb is: %d LD/n",asd);
	puts(temp);

	char buf[MAX_BUF]={0};
	while(buf[0] == 0){
		receiveString(con,buf,MAX_BUF);
	}
	printf("%s\n", buf);

	puts("Stock of papa?");
	sendString(con, "get_stock_of papa");

	buf[0] = 0;
	while(buf[0] == 0){
		receiveString(con,buf,MAX_BUF);
	}

	sendString(con, END_OF_CONNECTION);

	printf("%s\n",buf);

	sendString(con, "String mandado por cliente\0");
}
