//
// Created by juanfra on 30/09/16.
//


#include "log.h"


#define MAX_BUFFER 300
#define LOG "log-"

int main(int argc, char *argv[]){


    if(argc<2){
        puts("ERROR NO FILE TO CONNECT");
        exit(1);
    }
    //Given File for creation by argument
    int key = ftok(argv[1], 'A');
    int msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("ERROR OPENING QUEUE");
        exit(0);
    }

    char filetime[MAX_BUFFER]={0};
    char buf[MAX_BUFFER]={0};
    struct tm *ts;
    time_t now;


    //Creating log file name
    now = time(0);
    ts = localtime(&now);
    strftime(filetime, sizeof(filetime), "%Y-%m-%d_%H:%M:%S", ts);
    strcat(buf,"Logs/");
    strcat(buf+strlen(buf),LOG);
    strcat(buf+strlen(buf),filetime);
    FILE* log=fopen(buf,"w");

    //initialize
    fprintf(log,"INITIALIZE LOG OF %s\n",filetime);
    fflush(log);


    msgbuf_t msg;
    int ans=0;

    //waiting for queue to open
    msgrcv(msqid,&msg,sizeof(msg),0,0);

    do{
        if(ans!=0 && ans!=-1) memset(&msg, 0, sizeof(msg));

        ans=0;

        //Checks msg in order
        ans=msgrcv(msqid, &msg, sizeof(msgbuf_t), MERROR, MSG_NOERROR | IPC_NOWAIT);

        if(ans==0 || ans==-1){
            ans=msgrcv(msqid, &msg, sizeof(msgbuf_t), WARNING, MSG_NOERROR | IPC_NOWAIT);
        }

        if(ans==0 || ans==-1){
            ans=msgrcv(msqid, &msg, sizeof(msgbuf_t), INFO, MSG_NOERROR | IPC_NOWAIT);
        }

        switch((int)msg.mtype){
            case MERROR:
                fprintf(log,"ERROR: ");
                break;
            case WARNING:
                fprintf(log,"WARNING: ");
                break;
            case INFO:
                fprintf(log,"INFO: ");
                break;
            default:
                break;
        }

        if(ans!=0 && ans!=-1){
            fprintf(log,"%s\n",msg.message);
            fflush(log);
        }

    }while(ans==0 || ans==-1 || strcmp(msg.message,"end of log")!=0);

    fclose(log);
    msgctl(msqid, IPC_RMID, NULL);

}
