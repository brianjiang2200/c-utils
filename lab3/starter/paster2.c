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

typedef struct DingLirenWC {
	Buffer* shared_buf;
	sem_t* shared_spaces;
	sem_t* shared_items;
	int pindex;
	int cindex;
	pthread_mutex_t* shared_mutex;
	int num_produced;
	int num_consumed;
} multipc;

<<<<<<< HEAD
int consumer(multipc* pc, struct chunk** all_IDAT, int sleep_time);
int producer(multipc* pc);
=======
int consumer(multipc* pc, struct chunk** all_IDAT);
int producer(multipc* pc, int img_no);
>>>>>>> ebd184f574781ba6dce9ea7ef8be5e235955c618

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

	curl_global_init(CURL_GLOBAL_DEFAULT):

	/*array of inflated IDAT data*/
	int IDAT_shmid = shmget(IPC_PRIVATE, 50 * sizeof(struct chunk*), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	/*Init all required shared multipc elements*/
	int multipc_shmid = shmget(IPC_PRIVATE, sizeof(multipc), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	/*fail if error*/
	if (multipc_shmid == -1 || IDAT_shmid == -1) {
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
				sem_init(shared_multipc->shared_spaces, 1, buf_size);
				sem_init(shared_multipc->shared_items, 1, 0);
				pthread_mutex_init(shared_multipc->shared_mutex, NULL);
				shared_multipc->pindex = 0;
				shared_multipc->cindex = 0;
				shared_multipc->num_produced = 0;
                                shared_multipc->num_consumed = 0;
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
			void* IDAT_tmp = shmat(IDAT_shmid, NULL, 0);
			if (multipc_tmp == (void*) -1 || IDAT_tmp == (void*) -1) {
				perror("shmat");
				abort();
			}
			multipc* shared_multipc = (multipc*) multipc_tmp;
			struct chunk** shared_IDAT = (struct chunk**) IDAT_tmp;
			/*perform all consumer work here*/
			consumer(shared_multipc, shared_IDAT, sleep_time);
			if (shmdt(multipc_tmp) != 0 || shmdt(IDAT_tmp) != 0) {
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

	curl_global_cleanup();

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

int consumer(multipc* pc, struct chunk** all_IDAT, int sleep_time) {
	printf("Consumer working!\n");

	pthread_mutex_lock(mutex);
	int num_consumed = pc->num_consumed;	//will this temp variable change before while loop?
	pthread_mutex_unlock(mutex);

	while(num_consumed < 50) {

		wait(pc->shared_spaces);

		pthread_mutex_lock(mutex);

		//sleep for specified amount of s = ms/1000
		sleep(sleep_time / 1000);

		//read image segments out of buffer into IDAT chunk array
		// = pc->shared_buf->tail->buf

		//pop the image read (tail) from buffer
		Buffer_pop(pc->shared_buf);


	}

	return 0;
}

/*ALGORITHM:
	For each image segment (1-50):
		decrement items (if not 0, else wait until not 0)

*/

int producer(multipc* pc, int img_no) {
	printf("Producer working!\n");
	CURL *curl_handle;
	curl_handle = curl_easy_init();
	if (curl_handle == NULL) {
		return -1;
	}
	CURLcode res;
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_cb_curl3);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_cb_curl);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	while(pc->num_produced < 50) {
		RECV_BUF recv_buf;
		recv_buf_init(&recv_buf, 10000);
		char url[64];
		sprintf(url, "%s%d&part=%d", IMG_URL, img_no, num_produced);

		curl_easy_setopt(curl_handle, CURL_URL, url);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&recv_buf);
		curl_easy_setopt(curl_handle, CURL_HEADERDATA, (void*)&recv_buf);

		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK) {
			recv_buf_cleanup(&recv_buf);
			curl_easy_cleanup(curl_handle);
			return -2;
		}

		sem_wait(pc->shared_spaces);
		pthread_mutex_lock(pc->shared_mutex);
		buffer_add(pc->shared_buf, &recv_buf);
		pthread_mutex_unlock(pc->shared_mutex);
		sem_post(&items);

		pc->num_produced++;
	}
	curl_easy_cleanup(curl_handle);

	return 0;
}
