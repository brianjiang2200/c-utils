#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <curl/curl.h>
#include <getopt.h>
#include "catpng.h"
#include "main_write_header_cb.h"

#define IMG_URL "http://ece252-1.uwaterloo.ca:2520/image?img="
#define ECE252_HEADER "X-Ece252-Fragment: "
#define BUF_SIZE 1048576
#define BUF_INC 524288

int main(int argc, char** argv) {
	int c;
	int no_threads = 1;
	int img_no = 1;

	/*check arguments*/
	while ((c = getopt(argc, argv, "t:n:")) != -1) {
		switch(c) {
		case 't':
			no_threads = strtoul(optarg, NULL, 10);
			break;
		case 'n':
			img_no = strtoul(optarg, NULL, 10);
			break;
		default:
			break;
		}
	}

	/*setup CURL*/
	CURL *curl_handle;
	CURLcode res;
	char url[256];
	sprintf(url, "%s%d", IMG_URL, img_no);
	RECV_BUF recv_buf;
	recv_buf_init(&recv_buf, BUF_SIZE);

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl_handle = curl_easy_init();
	if (curl_handle == NULL) {
		return -1;
	}

	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	/*callback function to process received data*/
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_cb_curl3);
	/*set recv buffer*/
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&recv_buf);
	/*callback to process received header data*/
	curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_cb_curl);
	/*set recv buffer*/
	curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void*)&recv_buf);

	/*some servers may require a user-agent field*/
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/*map of retreived images*/
	int retrieved[50];
	memset(retrieved, 0, 50*sizeof(int));
	int num_retrieved = 0;

	while (num_retrieved < 50) {
		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK) {
			return -2;
		}
		if (!retrieved[recv_buf.seq]) {
			char fname[256];
			sprintf(fname, "./output_%d.png", recv_buf.seq);
			write_file(fname, recv_buf.buf, recv_buf.size);
			retrieved[recv_buf.seq] = 1;
			/*clean buf and re-init*/
			/*recv_buf_cleanup(&recv_buf);
			recv_buf_init(&recv_buf, BUF_SIZE);*/
		}
		num_retrieved++;
	}

	curl_easy_cleanup(curl_handle);

	curl_global_cleanup();
	recv_buf_cleanup(&recv_buf);

	return 0;
}

/*ALGORITHM:
	Retrieve all segments using cURL
	Output them to files
	Concatenate them using catpng method
*/
