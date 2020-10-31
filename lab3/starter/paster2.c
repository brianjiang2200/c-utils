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
#include "buffer.h"
#include "zutil.h"
#include "lab_png.h"
#include "main_2proc.h"

#define IMG_URL "http://ece252-1.uwaterloo.ca:2530/image?img="
#define ECE252_HEADER "X-Ece252-Fragment: "
#define IMG_SIZE 10240

typedef struct DingLirenWC {
	sem_t shared_spaces;
	sem_t shared_items;
	sem_t buffer;
	sem_t download;
	sem_t consumed;
	pthread_mutex_t shared_mutex;
	int num_produced;
	int num_consumed;
} multipc;

int consumer(Buffer* b, multipc* pc, struct chunk** all_IDAT, int sleep_time);
int producer(Buffer* b, multipc* pc, int img_no);

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

	int shared_buf_shmid = shmget(IPC_PRIVATE, sizeof_Buffer(buf_size, IMG_SIZE), 0666 | IPC_CREAT);
	/*array of inflated IDAT data*/
	int IDAT_shmid = shmget(IPC_PRIVATE, 50 * sizeof(struct chunk*), 0666 | IPC_CREAT);
	/*Init all required shared multipc elements*/
	int multipc_shmid = shmget(IPC_PRIVATE, sizeof(multipc), 0666 | IPC_CREAT);
	/*fail if error*/
	if (multipc_shmid == -1 || IDAT_shmid == -1 || shared_buf_shmid == -1) {
		perror("shmget");
		abort();
	}
	/*Must init shared memory elements prior to any processes are generated*/
	multipc* shared_multipc = (multipc*) shmat(multipc_shmid, NULL, 0);
	Buffer* shared_buf = (Buffer*) shmat(shared_buf_shmid, NULL, 0);
	struct chunk** IDAT_arr = (struct chunk**) shmat(IDAT_shmid, NULL, 0);
	if (shared_multipc == (multipc*)-1 || shared_buf == (Buffer*)-1 || IDAT_arr == (struct chunk**)-1) {
		perror("shmat");
		abort();
	}
	Buffer_init(shared_buf, buf_size, sizeof_shm_recv_buf(IMG_SIZE));
	sem_init(&(shared_multipc->shared_spaces), 1, buf_size);
	sem_init(&(shared_multipc->shared_items), 1, 0);
	sem_init(&(shared_multipc->buffer), 1, 1);
	sem_init(&(shared_multipc->download), 1, 1);
	sem_init(&(shared_multipc->consumed), 1, 1);
	pthread_mutex_init(&(shared_multipc->shared_mutex), NULL);
	shared_multipc->num_produced = 0;
	shared_multipc->num_consumed = 0;

	/*Check Addresses*/
//	printf("Address of Buffer: %p\n", (void*)shared_buf);
//	printf("Address of Buffer Queue %p\n", (void*)shared_buf->queue);
//	printf("Address of Buffer Queue[0].buf %p\n", (void*)shared_buf->queue[0].buf);

	/*Do work here*/
	/*Initialize producer processes*/
	pid_t* prod_ids = malloc(no_producers * sizeof(pid_t));
	for (int i = 0; i < no_producers; ++i) {
		if ((prod_ids[i] = fork()) < 0) {
			perror("fork");
			abort();
		} else if (prod_ids[i] == 0) {
			/*perform all producer work here*/
			if (producer(shared_buf, shared_multipc, img_no) != 0) {
				printf("Producer failed\n");
				return -1;
			}
			if (shmdt(shared_multipc) != 0 || shmdt(shared_buf) != 0) {
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
			/*perform all consumer work here*/
			if(consumer(shared_buf, shared_multipc, IDAT_arr, sleep_time) != 0) {
				printf("Consumer failed\n");
				return -1;
			}
			if (shmdt(shared_multipc) != 0 || shmdt(IDAT_arr) != 0 || shmdt(shared_buf) != 0) {
				perror("shmdt");
				abort();
			}
			return 0;
		}
	}

	/*wait for all child processes to finish*/
	int status;
	for (int i = 0; i < no_producers; ++i) {
		waitpid(prod_ids[i], &status, 0);
		printf("Producer %d exited with status: %d\n", i, status);
	}
	for (int i = 0; i < no_consumers; ++i) {
		waitpid(cons_ids[i], &status, 0);
		printf("Consumer %d exited with status: %d\n", i, status);
	}

//TEST
//	printf("MAIN: IDAT_arr[0]->p_data: %s\n", (char*)IDAT_arr[0]->p_data[0]);
//

	//CATPNG PROCESS (assuming all .png segments exist in directory)
	char* filenames[51];
	filenames[0] = malloc(20*sizeof(char));
	sprintf(filenames[0], "first");
	for(int i = 1; i < 51; i++) {
		filenames[i] = malloc(20*sizeof(char));
		sprintf(filenames[i], "output_%d.png", i - 1);
	}
	if(catpng(51, filenames) != 0) {
		perror("catpng");
		for (int i = 0; i < 51; ++i) {
			remove(filenames[i]);
			free(filenames[i]);
		}
	}
        for(int i = 0; i < 51; i++) {
		remove(filenames[i]);
		free(filenames[i]);
	}
	//(FROM LAB 2: paster.c)

	//destroy mutex and semaphores
	sem_destroy(&shared_multipc->shared_spaces);
	sem_destroy(&shared_multipc->shared_items);
	sem_destroy(&shared_multipc->sem_nop);
	sem_destroy(&shared_multipc->sem_noc);
	pthread_mutex_destroy(&shared_multipc->shared_mutex);

	/*Initialize all.png chunks after work has been performed*/
	if (shmdt(shared_multipc) != 0 || shmdt(shared_buf) != 0 || shmdt(IDAT_arr) != 0) {
		perror("shmdt");
		abort();
	}

	/*Clean up Shared Mem Segments*/
	if (shmctl(IDAT_shmid, IPC_RMID, NULL) == -1 || shmctl(multipc_shmid, IPC_RMID, NULL) == -1 ||
		shmctl(shared_buf_shmid, IPC_RMID, NULL) == -1) {
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

int consumer(Buffer* b, multipc* pc, struct chunk** all_IDAT, int sleep_time) {

	int k = 0;

	while(k < 50) {
		usleep(sleep_time * 1000);

		sem_wait(&pc->sem_noc);
		k = pc->num_consumed;
		pc->num_consumed++;
		sem_post(&pc->sem_noc);

		sem_wait(&pc->shared_items);
		printf("CONSUMER: consumer %d got the go ahead\n", k);
		pthread_mutex_lock(&pc->shared_mutex);

		RECV_BUF* data = &b->queue[b->rear];

		//Create the image segment PNG file
		char fname[32];
		sprintf(fname, "output_%d.png", data->seq);
		write_file(fname, data->buf, data->size);


		//Pop the image read from the queue
		Buffer_pop(b);
		printf("CONSUMER: consumed item %d from the buffer\n", k);
		//Increment number of images processed

		pthread_mutex_unlock(&pc->shared_mutex);

		sem_post(&pc->shared_spaces);
		//POST (unlock and increment number of spaces)
	}

	return 0;
}

int producer(Buffer* b, multipc* pc, int img_no) {
	CURL *curl_handle;
	curl_handle = curl_easy_init();
	if (curl_handle == NULL) {
		return -1;
	}
	CURLcode res;
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_cb_curl);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_cb_curl);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	int k = 0;

	while(k < 50) {

		sem_wait(&pc->sem_nop);
		k = pc->num_produced;
		pc->num_produced++;
		sem_post(&pc->sem_nop);

		RECV_BUF* recv_buf = (RECV_BUF*) malloc(sizeof_shm_recv_buf(IMG_SIZE));
		if (shm_recv_buf_init(recv_buf, IMG_SIZE) != 0) perror("recv_buf_init");
		char url[64];
		sprintf(url, "%s%d&part=%d", IMG_URL, img_no, k);

		curl_easy_setopt(curl_handle, CURLOPT_URL, url);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)recv_buf);
		curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void*)recv_buf);

		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK) {
			shm_recv_buf_cleanup(recv_buf);
			free(recv_buf);
			curl_easy_cleanup(curl_handle);
			return -2;
		}

		sem_wait(&pc->shared_spaces);
		pthread_mutex_lock(&pc->shared_mutex);

		Buffer_add(b, recv_buf, IMG_SIZE);

		printf("PRODUCER: added img %d to the buffer: buffer size %d and seq num: %d\n", k,
			b->size, recv_buf->seq);

		pthread_mutex_unlock(&pc->shared_mutex);
		sem_post(&pc->shared_items);

		shm_recv_buf_cleanup(recv_buf);
		free(recv_buf);
	}
	curl_easy_cleanup(curl_handle);

	return 0;
}
