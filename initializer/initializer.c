#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include "../lib/global.h"

int main(int argc, char *argv[])
{
	printf("Main PID: %d\n",getpid());

	printf("[Initializer]: Start\n");

	// Se solicitan la cantidad de líneas por crear.
	int nLines;
	char nLinesStr[255];
	printf("Number of memory spaces to be created: ");
	fgets(nLinesStr, sizeof(nLinesStr), stdin);
	sscanf(nLinesStr, "%d", &nLines);

	printf("[Initializer]: Requesting resources\n");

	// 1. Creación del KEY
	int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error with key\n");
		return -1; 
	}

	// 2. Creación de zona compartida
	int id_zone = shmget (key, sizeof(struct sm_node)*(nLines+1), 0777 | IPC_CREAT);
	if (id_zone == -1) {
		fprintf (stderr, "Error with id_zone\n");
		return -1; 
	}

	printf ("[Initializer]: ID zone shared memory: %i\n", id_zone);

	// 3. Se solicita la memoria compartida
	struct sm_node * head = shmat (id_zone, (char *)0, 0);
	if (head == NULL) {
		fprintf (stderr, "Error reserve shared memory\n");
		return -1; 
	}

	// 4. Ingreso de información complementaria
	head[0].idLine = -1;
	head[0].owner = nLines;
	head[0].nSegment = -1;
	head[0].nPag_pSeg = -1;

	// 5. Se escribe en la memoria compartida
	// los valores default
	int i;
	for (i = 1; i < nLines+1; i++) {
		head[i].idLine = i-1;
		head[i].owner = 0;
		head[i].nSegment = 0;
		head[i].nPag_pSeg = 0;
	}

	printf("[Initializer]: End\n");
	return(0);
}