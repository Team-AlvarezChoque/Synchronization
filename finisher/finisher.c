#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/shm.h>
#include "../lib/global.h"

int main(int argc, char *argv[])
{
	printf("Main PID: %d\n",getpid());

	printf("[Finisher]: Start\n");

	printf("[Finisher]: Killing process of producer.\n");

	system("kill -9 $(pgrep productor) >/dev/null 2>&1");

	printf ("[Finisher]: Releasing shared memory\n");

	// 1. Creación del KEY
	int key = ftok(PATHKEY, KEY);
	if (key == -1) {
		fprintf (stderr, "Error with key\n");
		return -1; 
	}

	// 2. Creación de zona compartida
	int id_zone = shmget (key, sizeof(struct sm_node), 0777 | IPC_CREAT);
	if (id_zone == -1) {
		fprintf (stderr, "Error with id_zone\n");
		return -1; 
	}

	// 3. Se solicita la memoria compartida
	struct sm_node * head = shmat (id_zone, (char *)0, 0);
	if (head == NULL) {
		fprintf (stderr, "Error reserve shared memory\n");
		return -1; 
	}

	// 4. Liberar la memoria compartida
	shmdt ((char *)head);
	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);

	printf("[Finisher]: End\n");

	return(0);
}