#include <stdio.h>
#include <stdlib.h>
#include "lab_png.h"
#include <string.h>
#include <arpa/inet.h>

int is_png(U8 *buf, size_t n) {
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

	fread(out, DATA_IHDR_SIZE, 1, fp);

	out->width = ntohl(out->width);
	out->height = ntohl(out->height);

	rewind(fp);

	return 0;
}

U32 get_png_width(struct data_IHDR *buf) {
        return buf->width;
}

U32 get_png_height(struct data_IHDR *buf) {
	return buf->height;
}


int get_chunk(struct chunk *out, FILE *fp, U8 type[4]) {

	if(fp == NULL) {
		return -1;
	}

	long long int data = 0;

	switch(type[3]) {
		case 'R':
			get_png_data_IHDR(out, fp, 0, 0);
			break;
		case 'T':
                        fseek(fp, 33, SEEK_SET);
			fread(out->length, sizeof(U32), 1, fp);
			fread(out->type[0], 1, 1, fp);
                        fread(out->type[1], 1, 1, fp);
                        fread(out->type[2], 1, 1, fp);
                        fread(out->type[3], 1, 1, fp);

			fseek(fp, -16, SEEK_END);
			int data_size = ftell(fp) - 41;
                        fread(out->crc, sizeof(U32), 1, fp);

			fseek(fp, 41, SEEK_SET);
			fread(data, data_size, 1, fp);
			out->p_data = &data;
			break;
		case 'D':
			
			break;
	}

}


int main () {

	FILE *fp = fopen("../images/red-green-16x16.png", "r");

	if(fp == NULL) {
		return -1;
	}

	struct data_IHDR *temp = malloc(sizeof(struct data_IHDR));
	get_png_data_IHDR(temp, fp, 0, 0);

	printf("%u\n%u\n%u\n%u\n%u\n%u\n%u\n", temp->width, temp->height, temp->bit_depth, temp->color_type, 
		temp->compression, temp->filter, temp->interlace);

	free(temp);
	fclose(fp);

	return 0;
}


