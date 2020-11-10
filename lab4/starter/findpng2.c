#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <curl/curl.h>
#include <pthread.h>
#include <semaphore.h>
#include <search.h>
#include "curl_xml.h"

/*nodes in frontier linked list*/
typedef struct dummy1
{
	char* url;
	struct dummy1* next;
} frontier_node;

/*PNG Node*/
typedef struct dummy2
{
	char* url;
	struct dummy2* next;
} png_node;

int main(int argc, char** argv) {

	if (argc < 2) {
		return -1;
	}

	double times[2];
	struct timeval tv;

	/*record time before program execution*/
	if (gettimeofday(&tv, NULL) != 0) {
		perror("gettimeofday");
		abort();
	}
	times[0] = (tv.tv_sec) + tv.tv_usec/1000000.;

	/*arguments*/
	int no_threads = 1;
	int num_urls = 50;
	int logging = 0;
	char logfile[64];
	memset(logfile, 0, 64);
	int c;
	/*argv[argc - 1] is the seed URL*/

	while ((c = getopt(argc, argv, "t:m:v:")) != -1) {
		switch(c) {
		case 't':
			no_threads = strtoul(optarg, NULL, 10);
			break;
		case 'm':
			num_urls = strtoul(optarg, NULL, 10);
			break;
		case 'v':
			logging = 1;
			sprintf(logfile, optarg, sizeof(optarg));
			break;
		default:
			break;
		}
	}

	/*thread init*/
	pthread_t* threads = malloc(no_threads * sizeof(pthread_t));
	/*init URL frontier*/
	frontier_node* fhead = malloc(sizeof(frontier_node));
	memcpy(fhead->url, argv[argc-1], strlen(argv[argc-1]));
	fhead->next = NULL;
	/*init glib hash table for visited URLS*/
	hcreate(2 * num_urls);
	/*init PNG result list*/
	png_node* phead = NULL;
	unsigned int pngs_collected = 0;

	/*test code*/

	/*CURL *curl_handle;
	CURLcode res;
	RECV_BUF recv_buf;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl_handle = easy_handle_init(&recv_buf, argv[argc-1]);
	if (curl_handle == NULL) {
		abort();
	}
	res = curl_easy_perform(curl_handle);
	process_data(curl_handle, &recv_buf);
	cleanup(curl_handle, &recv_buf);*/

	/*test code ends here*/

	/*record time after program execution is finished*/
	if (gettimeofday(&tv, NULL) != 0) {
		perror("gettimeofday");
		abort();
	}
	times[1] = (tv.tv_sec) + tv.tv_usec/1000000.;
	printf("findpng2 execution time: %.6lf seconds\n", times[1] - times[0]);

	free(threads);
	free(fhead);
	hdestroy();

	return 0;
}

