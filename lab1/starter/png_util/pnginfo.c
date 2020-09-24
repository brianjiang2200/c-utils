#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab_png.h"
#include "crc.h"

unsigned long expected_CRC(struct chunk* buf) {
	U8 crc_buf[30000];
	memcpy(crc_buf, &buf->type, 4);
	memcpy(crc_buf, buf->p_data, buf->length);
	return crc(crc_buf, 4 + buf->length);
}

/*To be compiled into pnginfo*/
int main(int argc, char** argv) {
	char* filename = argv[1];
	struct data_IHDR* buf = malloc(sizeof(struct data_IHDR));
	FILE *fp = fopen (filename, "r");
	if (fp == NULL) {
		return -1;
	}

	/*check png signature*/
	U8 PNG_sign[8];
	fread(&PNG_sign, sizeof(PNG_sign), 1, fp);
	rewind(fp);

	if(is_png(PNG_sign, 0) < 0) {
		printf("%s: Not a PNG file\n", filename);
		return -1;
	}

	/*if PNG, get dimensions*/
	get_png_data_IHDR(buf, fp, 0, 0);
	printf("%s: %u x %u\n", filename, buf->width, buf->height);

	/*check for crc corruption*/
	struct chunk *chunk_buf = malloc(sizeof(struct chunk));

	get_chunk(chunk_buf, fp, 0);
	unsigned long exp_crc = expected_CRC(chunk_buf);
	if (chunk_buf->crc != exp_crc) {
		printf("IHDR chunk CRC error: computed %08lx, expected %08x\n", exp_crc, chunk_buf->crc);
	}

	get_chunk(chunk_buf, fp, 1);
	exp_crc = expected_CRC(chunk_buf);
	if (chunk_buf->crc != exp_crc) {
		printf("IDAT chunk CRC error: computed %08lx, expected %08x\n", exp_crc, chunk_buf->crc);
	}

	get_chunk(chunk_buf, fp, 2);
	exp_crc = expected_CRC(chunk_buf);
	if (chunk_buf->crc != exp_crc) {
		printf("IEND chunk CRC error: computed %08lx, expected %08x\n", exp_crc, chunk_buf->crc);
	}

	fclose(fp);
	free(buf);
	free(chunk_buf);
	return 0;
}
