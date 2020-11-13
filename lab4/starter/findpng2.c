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
#include <fcntl.h>
#include "curl_xml.h"
#include "findpng2.h"

#define CT_PNG "image/png"
#define CT_HTML "text/html"
#define CT_PNG_LEN 9
#define CT_HTML_LEN 9
#define URL_LENGTH 256

int logging = 0;

/*work to be performed by threads*/
void* work(void* arg) {
	thread_args *p_in = arg;
	ENTRY e, *ep;
	CURL *curl_handle;
	CURLcode res;

	while (p_in->fhead != NULL && *p_in->pngs_collected < p_in->target) {

		/*pop the next element in frontier*/
		frontier_node* popped = p_in->fhead;
		p_in->fhead = p_in->fhead->next;
		if (p_in->fhead == NULL) p_in->ftail = NULL;

		e.key = popped->url;
		e.data = (void*) *p_in->pngs_collected;
		ep = hsearch(e, FIND);

		/*if already in visited, break*/
		if (ep != NULL) {	//represents successful search
			free(popped->url);
			free(popped);
			continue;
		}
		ep = hsearch(e, ENTER);

		/*hsearch with ENTER flag enters the element if its not already there*/

		/*print the URL to log file*/
		if (logging) {
			FILE *fp = fopen(p_in->logfile, "a");
			if (fp != NULL) {
				fwrite(e.key, strlen(e.key), 1, fp);
			}
		}

		/*CURL the popped URL*/
        	RECV_BUF recv_buf;
		curl_handle = easy_handle_init(&recv_buf, popped->url);
		printf("GRABBING URL: %s\n", popped->url);
		if (curl_handle == NULL) {
			abort();
		}
		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK) {
			printf("curl_easy_perform() failed: %s \n", curl_easy_strerror(res));
			free(popped->url);
			free(popped);
			cleanup(curl_handle, &recv_buf);
			/*keep trying*/
			continue;
		}
		/*data processing handled externally*/
		process_data(curl_handle, &recv_buf, arg);

		free(popped->url);
		free(popped);
		cleanup(curl_handle, &recv_buf);

	}

	return NULL;
}

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
	fhead->url = malloc(URL_LENGTH * sizeof(char));
	memset(fhead->url, 0, URL_LENGTH * sizeof(char));
	memcpy(fhead->url, argv[argc-1], strlen(argv[argc-1]) * sizeof(char));
	fhead->next = NULL;
	frontier_node* ftail = fhead;
	/*init glib hash table for visited URLS*/
	hcreate(2 * num_urls);
	/*init PNG result list*/
	png_node* phead = NULL;
	int pngs_collected = 0;

	//SINGLE-THREADED
	thread_args *p_in = malloc(sizeof(thread_args));
	p_in->fhead = fhead;
	p_in->ftail = ftail;
	p_in->phead = phead;
	p_in->pngs_collected = &pngs_collected;
	p_in->target = num_urls;
	p_in->logfile = logfile;
	//

	/*MULTI-THREADED

	*/

	/*curl init*/
	curl_global_init(CURL_GLOBAL_DEFAULT);

	/*thread create*/
        pthread_create(threads, NULL, work, p_in);

	pthread_join(threads[0], NULL);

	/*Print PNG URLs to png.urls.txt, this will create an empty file even if nothing to print*/
	FILE* fp_pngs;
	fp_pngs = fopen("png_urls.txt", "w");
	png_node* png_stepper = p_in->phead;
	while (png_stepper != NULL) {
		fprintf(fp_pngs, "%s\n", png_stepper->url);
		png_stepper = png_stepper->next;
	}
	fclose(fp_pngs);

	/*record time after program execution is finished*/
	if (gettimeofday(&tv, NULL) != 0) {
		perror("gettimeofday");
		abort();
	}
	times[1] = (tv.tv_sec) + tv.tv_usec/1000000.;
	printf("findpng2 execution time: %.6lf seconds\n", times[1] - times[0]);

	/*CLEANUP*/
	curl_global_cleanup();
	/*destroy linked lists*/
	frontier_node* fstepper = p_in->fhead;
	while (fstepper != NULL) {
		frontier_node* tmp = fstepper;
		fstepper = fstepper->next;
		free(tmp->url);
		free(tmp);
	}
	png_node* pstepper = p_in->phead;
	while (pstepper != NULL) {
		png_node* tmp = pstepper;
		pstepper = pstepper->next;
		free(tmp->url);
		free(tmp);
	}
	free(p_in);
	free(threads);
	hdestroy();

	return 0;
}

