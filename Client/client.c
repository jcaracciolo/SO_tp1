#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../Server/Coms/coms.h"
#include "../Server/Marshalling/marsh.h"
#include "../Server/DB/UUID_DataBase/data_types.h"

#define END_OF_CONNECTION "KILLMEPLZ"

#define MAX_BUF 300


int main() {
	char hostname[MAX_BUF];
	char buffer[MAX_BUF];


	gethostname(hostname,MAX_BUF);

	gethostname(hostname,250);
	strcpy(buffer,"12352.");
	strcat(buffer,hostname);
	connection * con = connectToAddres(buffer);

	puts("Price of papa?");
	int pricePapa;
	sendInt(con, PRICE);
	pricePapa=receiveInt(con); //TODO: replace with ack
	sendString(con, "papa\0");
	pricePapa=receiveInt(con);
	printf("%d\n", pricePapa);

	puts("Stock of papa?");
	sendInt(con, STOCK);
	pricePapa=receiveInt(con); //TODO: replace with ack
	sendString(con, "papa");
	int stockPapa;
	stockPapa=receiveInt(con);
	printf("%d\n", stockPapa);

    puts("SENDING SELL");
    sendInt(con, SELL);
	sendInt(con, CLOSE);
    endConnection(con);


}
