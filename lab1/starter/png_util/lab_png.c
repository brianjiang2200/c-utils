#include <stdio.h>
#include <stdlib.h>
#include "lab_png.h"

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
	/*test test test*/
	return 0;
}

int get_png_data_IHDR(struct data_IHDR *out, FILE *fp, long offset, int whence) {
	return 0;
}

/*unit tests*/
int main () {
	U8 test_buffer[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x08};
	U8 bad_buffer[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x01, 0x1a, 0x04};
	printf("%d", is_png(test_buffer, 0));
	printf("%d", is_png(bad_buffer, 0));
	return 0;
}
