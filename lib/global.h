#ifndef GLOBAL_H
#define GLOBAL_H

#include <semaphore.h>
#include <fcntl.h>

// ----- Inicializador - Finalizador ----------

#define PATHKEY "/dev/null"
#define KEY 1300

struct sm_node {
	int idLine;
	unsigned long owner;
	int nSegment; // If all 0, its paging scheme
	int nPag_pSeg; // Page number or Segment part
};

struct thread_parms {
	sem_t * main_semaphore;
	sem_t * file_log;
	sem_t * file_dead;
	sem_t * file_end;
};

// -------------- Productor -------------------

#define MIN_AMOUNT_PAGES 1
#define MAX_AMOUNT_PAGES 10

#define MIN_AMOUNT_SEGMENTS 1
#define MAX_AMOUNT_SEGMENTS 5

#define MIN_AMOUNT_SPACES_SEGMENTS 1
#define MAX_AMOUNT_SPACES_SEGMENTS 3

#define MIN_TIME_PROCESS 1//20
#define MAX_TIME_PROCESS 3//60

#define MIN_TIME_CREATE_PROCESS 1//30
#define MAX_TIME_CREATE_PROCESS 3//60

#define SEMAPHORE "semaphore"

#define FILE_LOG "log.log"
#define FILE_DEAD "dead.log"
#define FILE_END "end.log"

#define BLOCK "block"
#define SEARCH "search"
#define SLEEP "sleep"

#define BUFF 1024

#define SEMAPHORE_FILE_LOG "semaphore_log"
#define SEMAPHORE_FILE_DEAD "semaphore_dead"
#define SEMAPHORE_FILE_END "semaphore_end"

#define TEST_BLOCK 0
#define SLEEP_BLOCK 1

#define TEST_SEARCH 0
#define SLEEP_SEARCH 1

#endif // GLOBAL_H
