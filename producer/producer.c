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
#include "../lib/global.h"
#include "../lib/asignacion.h"
#include "../lib/utils.h"

void INThandler(int);

int random_c(int min, int max){
	int num;
	num = rand();
	num = num % (max-min+1);
	return num + min;
}

void asignarSegmentacion(unsigned long PID,int nSegment, struct thread_parms * th_p){

	// Gestión de solicitud de memoria compartida

	// 1. Creación del KEY
	int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error with key\n");
		return -1; 
	}

	// 2. Acceso a la zona compartida
	int id_zone = shmget (key, sizeof(struct sm_node), 0777);
	if (id_zone == -1) {
		fprintf (stderr, "Shared memory has not been created.\n");
		return -1; 
	}

	// 3. Se solicita la memoria compartida
	struct sm_node * head = shmat (id_zone, (char *)0, 0);
	if (head == NULL) {
		fprintf (stderr, "Error reserve shared memory\n");
		return -1; 
	}

	writeProccess(PID,BLOCK);

	if(TEST_BLOCK){
		sleep(SLEEP_BLOCK);
	}

	// 1. Pedir semáforo --------------------------------------------------------------------
	// Inicia zona crítica
	sem_wait(th_p->main_semaphore);

	writeProccess(PID,SEARCH);

	if(TEST_SEARCH){
		sleep(SLEEP_SEARCH);
	}

	// 2. Buscar ubicación ------------------------------------------------------------------

	int successFlag = 1;
	char * data;
	asprintf(&data, "");

	int i,j;
	for(i = 0; i < nSegment; i++){
		// Obtener una cantidad aleatoria de la cantidad de espacios (lineas)
		int nSpacesSegments = random_c(MIN_AMOUNT_SPACES_SEGMENTS,MAX_AMOUNT_SPACES_SEGMENTS);
		printf("#lines:%d\n",nSpacesSegments);
		int index = cantidadEspaciosLibresContiguos(head,nSpacesSegments);
		if(index == -1){
			// No hay una cantidad de espacios contiguos disponibles
			// para el segmento actual
			successFlag = 0;
			break;
		}else{
			// Se tiene la cantidad para el segmento actual
			for(j=0;j<nSpacesSegments;j++){
				head[index+j].owner = (int)PID;
				head[index+j].nSegment = i+1;
				head[index+j].nPag_pSeg = j+1;

				char hour[8];
				getHour(hour);

				// 3. Escribir bitácora ------------------------------------------------------------------
				
				asprintf(&data, "%s%u\tassignment\t%s\tS\t%d-%d-%d\n",data,PID,hour,head[index+j].idLine,i+1,j+1);
			}
		}
	}

	if(successFlag == 0){
		// No hay espacio disponible para la solicitud
		printf("There is no space available for the process %u\n",PID);

		// Algo falló, desasignar todo lo creado por el proceso
		desasignarMemoria(head, PID);
	}

	// 4. Devolver semáforo ----------------------------------------------------------------
	// Finaliza zona crítica
	sem_post(th_p->main_semaphore);

	if(successFlag == 0){
		char hour[8];
		getHour(hour);
		// 3. Escribir bitácora ------------------------------------------------------------------

		char * data1;
		asprintf(&data1, "%u\tNo space available\t%s\tS\t--\n",PID,hour);

		sem_wait(th_p->file_log);
		writeFile(FILE_LOG,data1);
		sem_post(th_p->file_log);

		char * dataX1;
		asprintf(&dataX1,"%u, ",PID);
		sem_wait(th_p->file_dead);
		writeFile(FILE_DEAD,dataX1);
		sem_post(th_p->file_dead);

		removeFile(PID);

		return;
	}
	else{
		sem_wait(th_p->file_log);
		writeFile(FILE_LOG,data);
		sem_post(th_p->file_log);
	}

	writeProccess(PID,SLEEP);
	// 5. Sleep ----------------------------------------------------------------------------
	// Simulación de ejecución del proceso
	sleep(random_c(MIN_TIME_PROCESS,MAX_TIME_PROCESS));

	// Para este punto ya se inicializó la solicitud y acceso a memoria compartida
	// entonces sólo se vuelve a usar la memoria

	writeProccess(PID,BLOCK);

	// 6. Pedir semáforo -------------------------------------------------------------------
	// Inicia zona crítica
	sem_wait(th_p->main_semaphore);
	
	// 7. Liberar memoria ------------------------------------------------------------------
	desasignarMemoria(head, PID);

	char hour[8];
	getHour(hour);

	// 8. Escribir bitácora ----------------------------------------------------------------
	char * data2;
	asprintf(&data2, "%u\tde-assignment\t%s\tP\t--\n",PID,hour);

	sem_wait(th_p->file_log);
	writeFile(FILE_LOG,data2);
	sem_post(th_p->file_log);

	// 9. Devolver semáforo ----------------------------------------------------------------
	// Finaliza zona crítica
	sem_post(th_p->main_semaphore);

	char * dataX2;
	asprintf(&dataX2,"%u, ",PID);
	sem_wait(th_p->file_end);
	writeFile(FILE_END,dataX2);
	sem_post(th_p->file_end);

	removeFile(PID);
}

void asignar_proceso_segmentation(struct thread_parms * th_p){
	pthread_t id_thread;
	id_thread = pthread_self();
	printf("[THREAD]:%u\n", (int)id_thread);

	int nSegments = random_c(MIN_AMOUNT_SEGMENTS,MAX_AMOUNT_SEGMENTS);
	printf("Process with %d segments.\n",nSegments);
	asignarSegmentacion((int)id_thread, nSegments,th_p);

	pthread_cancel(id_thread); // Close the thread.
}

void asignarPaginacion(unsigned long PID,int nPages, struct thread_parms * th_p){

	// Gestión de solicitud de memoria compartida

	// 1. Creación del KEY
	int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error with key\n");
		return -1; 
	}

	// 2. Acceso a la zona compartida
	int id_zone = shmget (key, sizeof(struct sm_node), 0777);
	if (id_zone == -1) {
		fprintf (stderr, "Shared memory has not been created.\n");
		return -1; 
	}

	// 3. Se solicita la memoria compartida
	struct sm_node * head = shmat (id_zone, (char *)0, 0);
	if (head == NULL) {
		fprintf (stderr, "Error reserve shared memory\n");
		return -1; 
	}

	writeProccess(PID,BLOCK);

	if(TEST_BLOCK){
		sleep(SLEEP_BLOCK);
	}
	
	// 1. Pedir semáforo --------------------------------------------------------------------
	// Inicia zona crítica
	sem_wait(th_p->main_semaphore);

	writeProccess(PID,SEARCH);

	if(TEST_SEARCH){
		sleep(SLEEP_SEARCH);
	}

	// 2. Buscar ubicación ------------------------------------------------------------------
	// Algoritmo de asignación de paginación
	int freeSpace = cantidadEspaciosLibres(head);
	printf("Free spaces: %d.\n",freeSpace);
	// Hay espacio disponible para la solicitud
	if(nPages <= freeSpace){

		// Buscando espacios en donde meter las solicitudes
		int i, allocatedSpaces = 0;
		for(i = 1; i < head[0].owner +1; i++){
			// Espacio disponible
			if(head[i].owner == 0 && 
				head[i].nSegment == 0 && 
				head[i].nPag_pSeg == 0){

					// Asignacion
					allocatedSpaces++;
					head[i].owner = (int)PID;
					head[i].nSegment = 0;
					head[i].nPag_pSeg = allocatedSpaces;

					char hour[8];
					getHour(hour);

					// 3. Escribir bitácora ------------------------------------------------------------------
					char * data;
					asprintf(&data, "%u\tassignment\t%s\tP\t%d-%d\n",PID,hour,head[i].idLine,allocatedSpaces);

					//printf("%s\n",data);
					sem_wait(th_p->file_log);
					writeFile(FILE_LOG,data);
					sem_post(th_p->file_log);

					if(allocatedSpaces == nPages ){
						// Ya llené todos los espacios que ocupaba
						break;
					}
				}
		}

	}else{
		// No hay espacio disponible para la solicitud
		printf("There is no space available for the process %u\n",PID);

		char hour[8];
		getHour(hour);

		// 3. Escribir bitácora ------------------------------------------------------------------
		char * data;
		asprintf(&data, "%u\tNo space available\t%s\tP\t-\n",PID,hour);
		sem_wait(th_p->file_log);
		writeFile(FILE_LOG,data);
		sem_post(th_p->file_log);

		char * dataX1;
		asprintf(&dataX1,"%u, ",PID);
		sem_wait(th_p->file_dead);
		writeFile(FILE_DEAD,dataX1);
		sem_post(th_p->file_dead);

		removeFile(PID);

		// 4. Devolver semáforo ----------------------------------------------------------------
		// Cierre particular del semáforo
		sem_post(th_p->main_semaphore);
		return;
	}

	// 4. Devolver semáforo ----------------------------------------------------------------
	// Finaliza zona crítica
	sem_post(th_p->main_semaphore);

	writeProccess(PID,SLEEP);
	// 5. Sleep ----------------------------------------------------------------------------
	// Simulación de ejecución del proceso
	sleep(random_c(MIN_TIME_PROCESS,MAX_TIME_PROCESS));

	// Para este punto ya se inicializó la solicitud y acceso a memoria compartida
	// entonces sólo se vuelve a usar la memoria

	writeProccess(PID,BLOCK);
	// 6. Pedir semáforo -------------------------------------------------------------------
	// Inicia zona crítica
	sem_wait(th_p->main_semaphore);
	
	// 7. Liberar memoria ------------------------------------------------------------------
	desasignarMemoria(head, PID);

	char hour[8];
	getHour(hour);

	// 8. Escribir bitácora ----------------------------------------------------------------
	char * data;
	asprintf(&data, "%u\tde-assignment\t%s\tP\t-\n",PID,hour);

	sem_wait(th_p->file_log);
	writeFile(FILE_LOG,data);
	sem_post(th_p->file_log);

	// 9. Devolver semáforo ----------------------------------------------------------------
	// Finaliza zona crítica
	sem_post(th_p->main_semaphore);

	char * dataX2;
	asprintf(&dataX2,"%u, ",PID);
	sem_wait(th_p->file_end);
	writeFile(FILE_END,dataX2);
	sem_post(th_p->file_end);

	removeFile(PID);
}


void asignar_proceso_paging(struct thread_parms * th_p){
	pthread_t id_thread;
	id_thread = pthread_self();
	printf("[THREAD]:%u\n", (int)id_thread);

	int nPages = random_c(MIN_AMOUNT_PAGES,MAX_AMOUNT_PAGES);
	printf("Process with %d pages.\n",nPages);
	asignarPaginacion((int)id_thread, nPages,th_p);

	pthread_cancel(id_thread); // Close the thread.
}


void showHelp(char * programa){
	printf("Usage: %s [option]\n", programa);
	printf("Options:\n");
	printf("\t-s or -S to Segmentation\n");
	printf("\t-p or -P to Paging\n");
}

int main(int argc, char *argv[])
{
	printf("Main PID: %d\n",getpid());

	// Importante para que funcione el random
	srand(time(NULL));
	// Handler del Ctrl-C
	signal(SIGINT, INThandler);

	int segmentation = 0, paging = 0, i = 0;

	for (i = 0; i < argc; i++)
	{
		if(strcmp(argv[i],"--help") == 0){
			showHelp(argv[0]);
			return 1;
		}
		else if (strcmp(argv[i],"-s") == 0 || strcmp(argv[i],"-S") == 0){
			printf("Scheme: Segmentation\n");
			segmentation = 1;
		}
		else if (strcmp(argv[i],"-p") == 0 || strcmp(argv[i],"-P") == 0){
			printf("Scheme: Paging\n");
			paging = 1;
		}
	}

	if(segmentation + paging > 1){
		printf("Invalid number of parameters\n");
		showHelp(argv[0]);
		return 1;
	}
	else if(segmentation + paging == 0){
		printf("You must select a scheme\n");
		showHelp(argv[0]);
		return 1;
	}

	// Limpiar archivos de logs
	cleanFile(FILE_LOG);
	cleanFile(FILE_DEAD);
	cleanFile(FILE_END);

	// Eliminar el semáforo
	sem_unlink(SEMAPHORE);
	sem_unlink(SEMAPHORE_FILE_LOG);
	sem_unlink(SEMAPHORE_FILE_END);
	sem_unlink(SEMAPHORE_FILE_DEAD);
	system("ls -a | grep -P \"^.\\d+$\" | xargs -d\"\n\" rm >/dev/null 2>&1");

	sem_t * semvar;
	semvar = sem_open(SEMAPHORE,O_CREAT | O_EXCL, 0777, 1);

	sem_t * sem_f_log;
	sem_f_log = sem_open(SEMAPHORE_FILE_LOG,O_CREAT | O_EXCL, 0777, 1);

	sem_t * sem_f_dead;
	sem_f_dead = sem_open(SEMAPHORE_FILE_DEAD,O_CREAT | O_EXCL, 0777, 1);

	sem_t * sem_f_end;
	sem_f_end = sem_open(SEMAPHORE_FILE_END,O_CREAT | O_EXCL, 0777, 1);

	struct thread_parms th_p;
	th_p.main_semaphore = semvar;
	th_p.file_log = sem_f_log;
	th_p.file_dead = sem_f_dead;
	th_p.file_end = sem_f_end;

	while(1){
		pthread_t newthread;

		// Crear thread para el proceso
		if(segmentation == 1){
			if (pthread_create(&newthread , NULL, asignar_proceso_segmentation, &th_p) != 0)
				perror("pthread_create");
		}else if(paging == 1){
			if (pthread_create(&newthread , NULL, asignar_proceso_paging, &th_p) != 0)
				perror("pthread_create");
		}

		// entre ejecución de procesos
		int sleep_time = random_c(MIN_TIME_CREATE_PROCESS,MAX_TIME_CREATE_PROCESS);
		printf("Waiting %ds to new process\n",sleep_time);
		sleep(sleep_time);
	}

	
	return(0);
}

void INThandler(int sig)
{
	signal(sig, SIG_IGN);
	// Cerrar semáforo y borrarlo
	sem_unlink(SEMAPHORE);
	sem_unlink(SEMAPHORE_FILE_LOG);
	sem_unlink(SEMAPHORE_FILE_END);
	sem_unlink(SEMAPHORE_FILE_DEAD);
	system("ls -a | grep -P \"^.\\d+$\" | xargs -d\"\n\" rm >/dev/null 2>&1");
	exit(0);
}