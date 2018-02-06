#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include  <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "../lib/global.h"
#include "../lib/utils.h"

void estadoMemoria(){
	// 1. Creación del KEY
	int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error with key\n");
		return -1; 
	}

	// 2. Acceso a la zona compartida
	int id_zone = shmget (key, sizeof(struct sm_node), 0777);
	if (id_zone == -1) {
		fprintf (stderr, "Shared memory not created.\n");
		return -1; 
	}

	// 3. Se solicita la memoria compartida
	struct sm_node * head = shmat (id_zone, (char *)0, 0);
	if (head == NULL) {
		fprintf (stderr, "Error reserve shared memory\n");
		return -1; 
	}

	sem_t * semvar;
	semvar = sem_open(SEMAPHORE,O_RDWR, 0777, 1);

	// En el caso que el productor no esté corriendo,
	// se crea el semáforo
	if(semvar == NULL){
		semvar = sem_open(SEMAPHORE,O_CREAT | O_EXCL, 0777, 1);
	}

	sem_wait(semvar);

	printf("[Memory status]:\n");

	int i;
	for (i = 1; i < head[0].owner + 1; i++) 
		printf("\tidLine:%d,\towner:%u,\tnSegment:%d,\tnPag_pSeg:%d\n", head[i].idLine, head[i].owner, head[i].nSegment, head[i].nPag_pSeg);
	
	printf("\n\n");

	sem_post(semvar);
}

void estadoProcesos(){
	printf("[Process status]:\n");

	char * block;
	char * sleep;
	char * search;

	asprintf(&block,"");
	asprintf(&sleep,"");
	asprintf(&search,"");

	char buff[BUFF];
	size_t nread;

	DIR *dp;
	struct dirent *ep;
	dp = opendir ("../producer/");

	if (dp != NULL)
	{
		// Se itera el directorio de archivos
		while (ep = readdir (dp)){
			
			if( strcmp(ep->d_name,".") != 0 && strcmp(ep->d_name,"..") != 0 &&
				ep->d_name[0] == 46
			){
				char * filename;
				char * filecontent;
				
				FILE * fp;
				// Leer contenido
				asprintf(&filename, "../producer/%s",ep->d_name);
				fp = fopen(filename,"r");

				asprintf(&filecontent,"");

				while( (nread = fread(buff,1,sizeof(buff),fp)) > 0){
					asprintf(&filecontent,"%s%s",filecontent,buff);
				}

				if(strcmp(filecontent,"block")==0){
					asprintf(&block,"%s%s, ",block,ep->d_name);
				}else if(strcmp(filecontent,"search")==0){
					asprintf(&search,"%s%s, ",search,ep->d_name);
				}else if(strcmp(filecontent,"sleep")==0){
					asprintf(&sleep,"%s%s, ",sleep,ep->d_name);
				}

				memset(buff,0,BUFF);
			}
		}

		(void) closedir (dp);
	}
	else
		perror ("Couldn't open the directory");


	printf("  [SLEEP]:%s\n",sleep);
	printf("  [SEARCH]:%s\n",search);
	printf("  [BLOCK]:%s\n",block);
	
	if(file_exist("../producer/dead.log")){
		sem_t * sem_f_dead;
		sem_f_dead = sem_open(SEMAPHORE_FILE_DEAD,O_RDWR, 0777, 1);

		// En el caso que el productor no esté corriendo,
		// se crea el semáforo
		if(sem_f_dead == NULL){
			sem_f_dead = sem_open(SEMAPHORE_FILE_DEAD,O_CREAT | O_EXCL, 0777, 1);
		}
		
		sem_wait(sem_f_dead);
		
		// Mostrar procesos que han muerto por falta espacio
		FILE * fp1;
		// Leer contenido
		fp1 = fopen("../producer/dead.log","r");
		char * DEAD;
		asprintf(&DEAD,"");
		while( (nread = fread(buff,1,sizeof(buff),fp1)) > 0){
			asprintf(&DEAD,"%s%s",DEAD,buff);
		}
		sem_post(sem_f_dead);

		printf("  [DEAD]:%s\n",DEAD);
	}
	else{
		printf("File dead.log does not exist\n");
	}

	if(file_exist("../producer/end.log")){
		sem_t * sem_f_end;
		sem_f_end = sem_open(SEMAPHORE_FILE_END,O_RDWR, 0777, 1);

		// En el caso que el productor no esté corriendo,
		// se crea el semáforo
		if(sem_f_end == NULL){
			sem_f_end = sem_open(SEMAPHORE_FILE_END,O_CREAT | O_EXCL, 0777, 1);
		}

		sem_wait(sem_f_end);
		// Mostrar procesos que han terminado satisfactoriamente
		FILE * fp2;
		// Leer contenido
		fp2 = fopen("../producer/end.log","r");
		char * END;
		asprintf(&END,"");
		while( (nread = fread(buff,1,sizeof(buff),fp2)) > 0){
			asprintf(&END,"%s%s",END,buff);
		}
		sem_post(sem_f_end);
		printf("  [END]:%s\n",END);
	}
	else{
		printf("File end.log does not exist\n");
	}
	
}

int main(int argc, char *argv[])
{
	printf("Main PID: %d\n",getpid());

	estadoMemoria();
	estadoProcesos();

	return(0);
}