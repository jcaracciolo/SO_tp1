#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../Server/Marshalling/marsh.h"
#include "../Server/DB/UUID_DataBase/data_types.h"

#define END_OF_CONNECTION "KILLMEPLZ"

#define MAX_BUF 300


int main() {
	char hostname[MAX_BUF];
	char buffer[MAX_BUF];
	char temp[MAX_BUF];

	gethostname(hostname,MAX_BUF);
	char hostname[250];
	char buffer[250];
	// char temp[250];

	gethostname(hostname,250);
	strcpy(buffer,"12352.");
	strcat(buffer,hostname);
	connection * con = connect(buffer);

	puts("Price of papa?");
	int pricePapa;
	sendInt(con, PRICE);
	receiveInt(con,&pricePapa); //TODO: replace with ack
	sendString(con, "papa\0");
	receiveInt(con,&pricePapa);
	printf("%d\n", pricePapa);

	puts("Stock of papa?");
	sendInt(con, STOCK);
	receiveInt(con,&pricePapa); //TODO: replace with ack
	sendString(con, "papa");
	int stockPapa;
	receiveInt(con,&stockPapa);
	printf("%d\n", stockPapa);

	sendInt(con, CLOSE);

}
