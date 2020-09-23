#include <stdio.h>
#include <stdlib.h>
#include "lab_png.h"

int is_png(U8 *buf, size_t n) {
	printf("%d\n", *buf);
	printf("%d\n", *(buf + 1));
	//check the first 8 bytes
	if (*buf != 0x89 ||
		*(buf + 1) != 0x50 ||
		*(buf + 2) != 0x4e ||
		*(buf + 3) != 0x47 ||
		*(buf + 4) != 0x0d ||
		*(buf + 5) != 0x0a ||
		*(buf + 6) != 0x1a ||
		*(buf + 7) != 0x0a) {
			return -1;
	}
	return 0;
}

int get_png_data_IHDR(struct data_IHDR *out, FILE *fp, long offset, int whence) {

	if(fp == NULL) {
		return -1;
	}

	fseek(fp, 16, SEEK_SET);

	fread(&out->width, sizeof(U32), 1, fp);

	fread(&out->height, sizeof(U32), 1, fp);

	return 0;
}

U32 get_png_width(struct data_IHDR *buf) {
        return buf->width;
}

U32 get_png_height(struct data_IHDR *buf) {
	return buf->height;
}

/*unit tests*/
int main () {

	FILE *fp = fopen("lab1/starter/images/WEEF_1.png", "r");

	struct data_IHDR *temp;
	get_png_data_IHDR(temp, fp, 0, 0);
	printf("WIDTH: %u\nHEIGHT: %u\n", temp->width, temp->height);

	fclose(fp);

	return 0;
}

