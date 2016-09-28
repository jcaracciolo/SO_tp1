#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "Coms/coms.h"
#include "server.h"
#include "DB/SQlite/SQLparser.h"

#define PATHDBIN "/tmp/fifoDBserverIN"
#define PATHDBOUT "/tmp/fifoDBserverOut"

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
    char hostname[250];
    char buffer[250];

    dbdata_t* DBdata=malloc(sizeof(dbdata_t));
    connectDB(DBdata);
	gethostname(hostname,250);

	strcpy(buffer,"12352.");
	strcat(buffer,hostname);

	int serverFD = openAdress(buffer);
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

void initializeDB(dbdata_t * DBdata) {
    createTable(DBdata);
    insertIntoTable(DBdata, "papa", 8, 3);
}

int connectDB(dbdata_t* DBdata){

    mkfifo(PATHDBOUT,0666);
    mkfifo(PATHDBIN,0666);

    if (fork() == 0) {
        //child DATABASE
        close(0);
        close(1);
        close(2);
        open(PATHDBIN,O_RDONLY);
        open(PATHDBOUT,O_WRONLY);
        open(PATHDBOUT,O_WRONLY);
        char* ar[3]={"sqlite3","-echo",NULL};
        execv("./DB/SQlite/sqlite3",ar);
    } else {

        DBdata->fdin = open(PATHDBIN,O_WRONLY);
        DBdata->fdout = open(PATHDBOUT,O_RDONLY);

        initializeDB(DBdata);

        int price = getPrice(DBdata, "papa");
        int stock = getStock(DBdata, "papa");
        printf("Papa cuesta: %i\nPapa stock: %i\n", price, stock);

		puts("Modificando tabla...");

        changeValue(DBdata, "papa", 40, 60);
        price = getPrice(DBdata, "papa");
        stock = getStock(DBdata, "papa");
        printf("Papa cuesta: %i\nPapa stock: %i\n", price, stock);

        exitDB(DBdata);

    }

    return 0;



}
