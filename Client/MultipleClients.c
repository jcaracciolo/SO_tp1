//
// Created by juanfra on 29/09/16.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

int main(){
    char* ar[2]={"client",NULL};
    for (int i = 0; i < 100; ++i) {

	printf("generating %d\n",i);
        int childPID;

        struct sigaction sigchld_action = {
                .sa_handler = SIG_DFL,
                .sa_flags = SA_NOCLDWAIT
        };
        sigaction(SIGCHLD, &sigchld_action, NULL);

        if ((childPID = fork()) == 0) {
            // Child
            execv("client",ar);
            printf("Child fork failed\n");
            exit(1);

        }
        if(childPID<0){
            exit(1);
        }
    }

    exit(0);

}

