#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <curl/curl.h>
#include <getopt.h>

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

	FILE* merged = fopen("all.png", "w");
	fclose(merged);

	return 0;
}
