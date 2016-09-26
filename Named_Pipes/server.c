#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "coms.h"
#include "server.h"

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

typedef struct{
    int fdin;
    int fdout;
    sem_t* sem;
} dbdata_t;

int main(int argc, char *argv[]) {

    dbdata_t* DBdata=malloc(sizeof(dbdata_t));
    connectDB(DBdata);





	char hostname[250];
	char buffer[250];
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

int connectDB(dbdata_t* DBdata){

    mkfifo(PATHDBOUT,0666);
    mkfifo(PATHDBIN,0666);

    if (fork() == 0) {
        //child DATABASE
        close(0);
        close(1);
        close(2);
        open(PATHDBIN,O_RDONLY | O_NONBLOCK);
        open(PATHDBOUT,O_RDWR);
        open(PATHDBOUT,O_RDWR);

        char* ar[3]={"sqlite3",0,0};
        execv("./sqlite3",ar);
    }else{
        printf("Connecting Database...\n");
        DBdata->fdout = open(PATHDBOUT,O_RDONLY | O_NONBLOCK);
        DBdata->fdin = open(PATHDBIN,O_RDWR);


        while(1) {
            printf("Checking Database output...\n");
            char *msg = "create table hola(a int);insert into hola values(1);select * from hola;drop table hola;\n";
            char str[200] = {0};
            int a = 0;

            write(DBdata->fdin, msg, strlen(msg) + 1);
            while (a <= 0) {
                a = read(DBdata->fdout, str, 200);
            }
            if (strcmp("1\n", str) != 0) {
                printf("DATABASE CONNECTION ERROR!");
                printf("%s", str);
                exit(0);
            }
            printf("Database output connected\n");
        }

//        printf("Checking Database error output...\n");
//
//        char* msgerror="create table hola(a int);insert into hola values(1);select * from hola;drop table hola;";
//        write(DBdata->fdin,msgerror,strlen(msgerror)+1);
//        a=0;
//        while(a<=0) {
//            a = read(DBdata->fdout, str, 200);
//        }
//        printf("SDADAS\n");
//
//        if(strcmp("1\n",str)!=0){
//            printf("DATABASE CONNECTION ERROR!");
//            printf("%s",str);
//            exit(0);
//        }
//        printf("Database output connected\n");
//
//        printf("Database connected\n");

    }

    return 0;



}