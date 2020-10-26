/*Custom Implementation of a fixed size queue to store recieve data from a CURL callback*/
#include <stdio.h>
#include <stdlib.h>
#include "main_write_header_cb.h"
#include "buffer.h"

void Buffer_init(Buffer* b, int max_size) {
	b->size = 0;
	b->max_size = max_size;
	b->tail = NULL;
	b->head = NULL;
}

void Buffer_add(Buffer* b, RECV_BUF* node) {
	if (b->size >= b->max_size) {
		return;
	}
	
}


