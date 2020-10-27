#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <curl/curl.h>
#include <pthread.h>
#include <semaphore.h>
#include "catpng.h"
#include "main_write_header_cb.h"
#include "buffer.h"

#define IMG_URL "http://ece252-1.uwaterloo.ca:2530/image?img="
#define ECE252_HEADER "X-Ece252-Fragment: "

int consumer(multipc* pc);
int producer(multipc* pc);

typedef struct DingLirenWC {
	Buffer* shared_buf;
	sem_t* shared_spaces;
	sem_t* shared_items;
	int pindex;
	int cindex;
	pthread_mutex_t* shared_mutex;
} multipc;

int main(int argc, char** argv) {

	if (argc < 6) {
		return -1;
	}

	double times[2];
	struct timeval tv;

	/*record time before first process*/
	if (gettimeofday(&tv, NULL) != 0) {
		perror("gettimeofday");
		abort();
	}
	times[0] = (tv.tv_sec) + tv.tv_usec/1000000.;

	/*arguments*/
	int buf_size = strtoul(argv[1], NULL, 10);
	int no_producers = strtoul(argv[2], NULL, 10);
	int no_consumers = strtoul(argv[3], NULL, 10);
	int sleep_time = strtoul(argv[4], NULL, 10);
	int img_no = strtoul(argv[5], NULL, 10);

	/*array of inflated IDAT data*/
	struct chunk** IDAT_arr = malloc(50 * sizeof(struct chunk*));
	/*Init all required shared multipc elements*/
	int multipc_shmid = shmget(IPC_PRIVATE, sizeof(multipc), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	/*fail if error*/
	if (multipc_shmid == -1) {
		perror("shmget");
		abort();
	}

	/*Do work here*/
	/*Initialize producer processes*/
	pid_t* prod_ids = malloc(no_producers * sizeof(pid_t));
	for (int i = 0; i < no_producers; ++i) {
		if ((prod_ids[i] = fork()) < 0) {
			perror("fork");
			abort();
		} else if (prod_ids[i] == 0) {
			/*generate shared memory segments*/
			void *multipc_tmp = shmat(multipc_shmid, NULL, 0);
			/*fail if error*/
			if (multipc_tmp == (void*)-1) {
				perror("shmat");
				abort();
			}
			multipc* shared_multipc = (multipc*) multipc_tmp;
			/*Must init shared memory elements prior to work*/
			if (i == 0) {
				Buffer_init(shared_multipc->shared_buf, buf_size);
				sem_init(shared_multipc->shared_spaces, 0, buf_size);
				sem_init(shared_multipc->shared_items, 0, 0);
				pthread_mutex_init(shared_multipc->shared_mutex, NULL);
				shared_multipc->pindex = 0;
				shared_multipc->cindex = 0;
			}
			/*perform all producer work here*/
			producer(shared_multipc);
			if (shmdt(multipc_tmp) != 0) {
				perror("shmdt");
				abort();
			}
			return 0;
		}
	}
	/*Initialize consumer processes*/
	pid_t* cons_ids = malloc(no_consumers * sizeof(pid_t));
	for (int i = 0; i < no_consumers; ++i) {
		if ((cons_ids[i] = fork()) < 0) {
			perror("fork");
			abort();
		} else if (cons_ids[i] == 0) {
			/*generate shared memory segments*/
			void* multipc_tmp = shmat(multipc_shmid, NULL, 0);
			if (multipc_tmp == (void*) -1 ) {
				perror("shmat");
				abort();
			}
			multipc* shared_multipc = (multipc*) multipc_tmp;
			/*perform all consumer work here*/
			consumer(shared_multipc);
			if (shmdt(multipc_tmp) != 0) {
				perror("shmdt");
				abort();
			}
			return 0;
		}
	}

	/*wait for all child processes to finish*/
	int status;
	pid_t pid;
	for (int i = 0; i < no_producers; ++i) {
		pid = wait(&status);
	}
	for (int i = 0; i < no_consumers; ++i) {
		pid = wait(&status);
	}

	/*Initialize all.png chunks after work has been performed*/


	/*record time after all.png is output*/
	if (gettimeofday(&tv, NULL) != 0) {
		perror("gettimeofday");
		abort();
	}
	times[1] = (tv.tv_sec) + tv.tv_usec/1000000.;
	printf("paster2 execution time: %.6lf seconds\n", times[1] - times[0]);

	free(prod_ids);
	free(cons_ids);
	free(IDAT_arr);

	return 0;
}

/*ALGORITHM:
	Initialize global data structures storing all.png properties
	->IDAT data for each segment stored in an array
	Initialize a Buffer of size buf_size
	Fork no_producers processes
	Initialize no_consumers consumers
	Producers add image data to Buffer
	Consumers process data into global data structures and sleep for sleep_time ms
	Output all.png when done
*/

int consumer(multipc* pc) {
	printf("Consumer working!\n");
	return 0;
}

int producer(multipc* pc) {
	printf("Producer working!\n");
	return 0;
}
