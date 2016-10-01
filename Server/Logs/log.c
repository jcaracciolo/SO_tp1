//
// Created by juanfra on 30/09/16.
//


#include "log.h"


#define MAX_BUFFER 300
#define LOG "log-"

int main(){

    time_t now;
    struct tm *ts;
    char buf[MAX_BUFFER];

    now = time(0);

    ts = localtime(&now);
    strcpy(buf,LOG);
    strftime(buf+strlen(LOG), sizeof(buf), "%Y-%m-%d_%H:%M:%S", ts);

    FILE* log=fopen(buf,"w");

    fprintf(log,"INITIALIZE LOG OF %s\n",buf+strlen(LOG));

    int key = ftok("log", 'A');
    int msqid = msgget(key, 0666 | IPC_CREAT);
    printf("%d\n",key);

    msgbuf_t msg;
    int ans=0;
    do{
        memset(&msg, 0, sizeof(msg));
        ans=msgrcv(msqid, &msg, sizeof(msgbuf_t), MERROR, 0);

        if(ans==0){
            ans=msgrcv(msqid, &msg, sizeof(msgbuf_t), WARNING, 0);
        }

        if(ans==0){
            ans=msgrcv(msqid, &msg, sizeof(msgbuf_t), INFO, 0);
        }

        switch((int)msg.mtype){
            case MERROR:
                fprintf(log,"ERROR: ");
                printf("ERROR: ");
                break;
            case WARNING:
                fprintf(log,"WARNING: ");
                printf("WARNING: ");
                break;
            case INFO:
                fprintf(log,"INFO: ");
                printf("INFO: ");
                break;
        }

        fprintf(log,"%s\n",msg.message);
        printf("%s\n",msg.message);

    }while(ans==0 || strcmp(msg.message,"end of log")!=0);

    fclose(log);
    msgctl(msqid, IPC_RMID, NULL);

}
