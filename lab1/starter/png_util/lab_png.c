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


int get_chunk(struct chunk *chk, FILE *fp, int flag) {
	/*extract a chunk from the file fp into chk
	flag:
		0 for IHDR
		1 for IDAT
		2 for IEND
	*/

	if(fp == NULL) {
		return -1;
	}

	U8 data[256];
	int data_size = 0;

	switch(flag) {
		case 0:
			fseek(fp, 8, SEEK_SET);
			fread(&chk->length, sizeof(U32), 1, fp);
                        chk->length = ntohl(chk->length);

			fread(&chk->type[0], 1, 1, fp);
			fread(&chk->type[1], 1, 1, fp);
			fread(&chk->type[2], 1, 1, fp);
			fread(&chk->type[3], 1, 1, fp);

//			U8 data0[256];

			fread(data, chk->length, 1, fp);	/*ONLY POINTS TO FIRST BYTE OF DATA*/
			chk->p_data = data;	/*WORKS, BUT APPEARS AS WARNING*/

			fseek(fp, 29, SEEK_SET);
			fread(&chk->crc, sizeof(U32), 1, fp);
			chk->crc = ntohl(chk->crc);

			rewind(fp);

			break;

		case 1:
                        fseek(fp, 33, SEEK_SET);
			fread(&chk->length, sizeof(U32), 1, fp);
			chk->length = ntohl(chk->length);

			fread(&chk->type[0], 1, 1, fp);
                        fread(&chk->type[1], 1, 1, fp);
                        fread(&chk->type[2], 1, 1, fp);
                        fread(&chk->type[3], 1, 1, fp);

			fseek(fp, -16, SEEK_END);

			data_size = ftell(fp) - 41;

//			U8 data1[data_size];

			fread(&chk->crc, sizeof(U32), 1, fp);
			chk->crc = ntohl(chk->crc);

			fseek(fp, 41, SEEK_SET);
			fread(data, chk->length, 1, fp);	/*ONLY POINTS TO FIRST BYTE OF DATA*/
			chk->p_data = data;	/*WORKS, BUT APPEARS AS WARNING*/

			rewind(fp);

			break;

		case 2:
			fseek(fp, -12, SEEK_END);
                        fread(&chk->length, sizeof(U32), 1, fp);
                        chk->length = ntohl(chk->length);

                        fread(&chk->type[0], 1, 1, fp);
                        fread(&chk->type[1], 1, 1, fp);
                        fread(&chk->type[2], 1, 1, fp);
                        fread(&chk->type[3], 1, 1, fp);

                        fread(&chk->crc, sizeof(U32), 1, fp);
                        chk->crc = ntohl(chk->crc);

			chk->p_data = NULL;

			rewind(fp);

			break;

		default:
			return -1;
	}

	return 0;
}


/*int main () {

	FILE *fp = fopen("../images/red-green-16x16.png", "r");

	if(fp == NULL) {
		return -1;
	}

	struct data_IHDR *temp = malloc(sizeof(struct data_IHDR));
	get_png_data_IHDR(temp, fp, 0, 0);

	printf("%u\n%u\n%u\n%u\n%u\n%u\n%u\n", temp->width, temp->height, temp->bit_depth, temp->color_type, 
		temp->compression, temp->filter, temp->interlace);

	free(temp);


	struct chunk *temp1 = malloc(sizeof(struct chunk));
	get_chunk(temp1, fp, 0);

	printf("%u\n%c%c%c%c\n%p\n%08x\n", temp1->length, temp1->type[0], temp1->type[1], temp1->type[2],
		temp1->type[3], temp1->p_data, temp1->crc);

	printf("%p\n", temp1->p_data);

	free(temp1);


	fclose(fp);

	return 0;
}*/


