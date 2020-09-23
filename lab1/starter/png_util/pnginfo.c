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

	U8 PNG_sign[8];
	fread(&PNG_sign, sizeof(PNG_sign), 1, fp);
	rewind(fp);

	if(is_png(PNG_sign, 0) < 0) {
		printf("%s: Not a PNG file\n", filename);
		return -1;
	}

	get_png_data_IHDR(buf, fp, 0, 0);
	printf("%s: %u x %u\n", filename, buf->width, buf->height);
	fclose(fp);
	free(buf);
	return 0;
}
