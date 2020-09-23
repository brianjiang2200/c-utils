#include <stdio.h>
#include <stdlib.h>
#include "lab_png.h"

int main(int argc, char** argv) {
	char* filename = argv[1];
	struct data_IHDR* buf = malloc(sizeof(struct data_IHDR));
	FILE *fp = fopen (filename, "r");
	if (fp == NULL) {
		return -1;
	}
	fclose(fp);
	free(buf);
	return 0;
}
