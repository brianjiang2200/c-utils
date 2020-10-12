#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <curl/curl.h>
#include <getopt.h>
#include "catpng.h"

#define IMG_URL "http://ece252-1.uwaterloo.ca:2520/image?img="

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

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl_handle = curl_easy_init();
	if (curl_handle == NULL) {
		return -1;
	}

	curl_easy_setopt(curl_handle, CURLOPT_URL, url);

	res = curl_easy_perform(curl_handle);
	if (res != CURLE_OK) {
		printf("curl_easy_perform() failed");
	}
	curl_easy_cleanup(curl_handle);

	curl_global_cleanup();

	return 0;
}
