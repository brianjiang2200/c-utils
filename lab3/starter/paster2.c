#define _DEFAULT_SOURCE

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
#include "zutil.h"
#include "lab_png.h"

#define IMG_URL "http://ece252-1.uwaterloo.ca:2530/image?img="
#define ECE252_HEADER "X-Ece252-Fragment: "

typedef struct DingLirenWC {
	Buffer* shared_buf;
	sem_t shared_spaces;
	sem_t shared_items;
	int pindex;
	int cindex;
	pthread_mutex_t* shared_mutex;
	int num_produced;
	int num_consumed;
} multipc;

int consumer(multipc* pc, struct chunk** all_IDAT, int sleep_time);
int producer(multipc* pc, int img_no);

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

	curl_global_init(CURL_GLOBAL_DEFAULT);

	/*array of inflated IDAT data*/
	int IDAT_shmid = shmget(IPC_PRIVATE, 50 * sizeof(struct chunk*), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	/*Init all required shared multipc elements*/
	int multipc_shmid = shmget(IPC_PRIVATE, sizeof(multipc), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	/*fail if error*/
	if (multipc_shmid == -1 || IDAT_shmid == -1) {
		perror("shmget");
		abort();
	}
	/*Must init shared memory elements prior to any processes are generated*/
	void *multipc_dummy = shmat(multipc_shmid, NULL, 0);
	if (multipc_dummy == (void*)-1) {
		perror("shmat");
		abort();
	}
	multipc* deleted_multipc = (multipc*) multipc_dummy;
	Buffer_init(deleted_multipc->shared_buf, buf_size);
	sem_init(&(deleted_multipc->shared_spaces), 0, buf_size);
	sem_init(&(deleted_multipc->shared_items), 0, 0);
	pthread_mutex_init(deleted_multipc->shared_mutex, NULL);
	deleted_multipc->pindex = 0;
	deleted_multipc->cindex = 0;
	deleted_multipc->num_produced = 0;
	deleted_multipc->num_consumed = 0;
	if (shmdt(multipc_dummy) != 0) {
		perror("shmdt");
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
			/*perform all producer work here*/
			if (producer(shared_multipc, img_no) != 0) {
				printf("Producer failed\n");
				return -1;
			}
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
		printf("%d exited with status: %d\n", pid, status);
	}
	for (int i = 0; i < no_consumers; ++i) {
		pid = wait(&status);
		printf("%d exited with status: %d\n", pid, status);
	}

	/*Initialize all.png chunks after work has been performed*/
	void* IDAT_tmp = shmat(IDAT_shmid, NULL, 0);
	if (IDAT_tmp == (void*)-1) {
		perror("shmat");
		abort();
	}
	struct chunk** IDAT_arr = (struct chunk**) IDAT_tmp;
	for (int i = 0; i < 50; ++i) {
		if (IDAT_arr[i] != NULL) {
			printf("IDAT %d length: %u\n", i, IDAT_arr[i]->length);
		}
	}
	if (shmdt(IDAT_tmp) != 0) {
		perror("shmdt");
		abort();
	}

	/*Clean up Shared Mem Segments*/
	if (shmctl(IDAT_shmid, IPC_RMID, NULL) == -1 || shmctl(multipc_shmid, IPC_RMID, NULL) == -1 ) {
		perror("shmctl");
		abort();
	}

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

	while(pc->num_consumed < 50) {
		//Sleep for specified amount of time
		usleep(sleep_time * 1000);

		//If > 0, decrement and execute. If 0, wait until item exists
		sem_wait(&pc->shared_items);

//CRITICAL PROCESS 1
		pthread_mutex_lock(pc->shared_mutex);

		//Create the image segment PNG file
		char fname[20];
		sprintf(fname, "output_%d.png", pc->shared_buf->tail->buf->seq);
		write_file(fname, pc->shared_buf->tail->buf->buf, pc->shared_buf->tail->buf->size);

//END OF CRITICAL PROCESS 1
		pthread_mutex_unlock(pc->shared_mutex);

		//Open PNG file for reading
		FILE* sample = fopen(fname, "r");

		//Read header
		U8 header[8];
		fread(header, 8, 1, sample);

		//Validate the received image segment
		if(is_png(header, 8)) {
			perror("is_png");
			return -1;
		}

		//Read IDAT
                struct chunk* new_IDAT = malloc(sizeof(struct chunk));
		get_chunk(new_IDAT, sample, 1);

		//Inflate received IDAT data
		U8* inflated_data = malloc(30 * new_IDAT->length);
		U64 len_inf = 0;
		U64 src_length = new_IDAT->length;
		int ret = mem_inf(inflated_data, &len_inf, new_IDAT->p_data, src_length);
		if (ret) {	/*failure*/
			/*clean up*/
			printf("Mem Inf Error: Return value %d\n", ret);
			return ret;
		}

//CRITICAL PROCESS 2
		pthread_mutex_lock(pc->shared_mutex);

		//Copy inflated data into proper place in memory
		all_IDAT[pc->shared_buf->tail->buf->seq] = new_IDAT;	//NEEDS TO BE inflated_data

		//Pop the image read from the queue
		Buffer_pop(pc->shared_buf);

		//Increment number of images processed
		pc->num_consumed++;

//END OF CRITICAL PROCESS 2
		pthread_mutex_unlock(pc->shared_mutex);

		//Increments number of spaces
		sem_post(&pc->shared_spaces);
	}

	return 0;
}

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

	pthread_mutex_lock(pc->shared_mutex);
	int k = pc->num_produced;
	pthread_mutex_unlock(pc->shared_mutex);

	while(k < 50) {
		RECV_BUF recv_buf;
		recv_buf_init(&recv_buf, 10000);
		char url[64];
		sprintf(url, "%s%d&part=%d", IMG_URL, img_no, k);

		curl_easy_setopt(curl_handle, CURLOPT_URL, url);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&recv_buf);
		curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void*)&recv_buf);

		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK) {
			recv_buf_cleanup(&recv_buf);
			curl_easy_cleanup(curl_handle);
			return -2;
		}

		sem_wait(&pc->shared_spaces);
		pthread_mutex_lock(pc->shared_mutex);
		Buffer_add(pc->shared_buf, &recv_buf);

		pc->num_produced++;
		k = pc->num_produced;
		pthread_mutex_unlock(pc->shared_mutex);
		sem_post(&pc->shared_items);
	}
	curl_easy_cleanup(curl_handle);

	return 0;
}
