/*Custom Implementation of a fixed size queue to store recieve data from a CURL callback*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include "buffer.h"
#include "main_2proc.h"

void Buffer_init(Buffer* b, int max_size, int recv_buf_size) {
	/*address space should be allocated by user*/
	if (b == NULL) {
		return;
	}
	b->size = 0;
	b->max_size = max_size;
	b->front = -1;
	b->rear = -1;
	b->queue = (RECV_BUF*)((char*) b + sizeof(Buffer));
	/*init all queue slots*/
	for (int i = 0; i < max_size; ++i) {
		RECV_BUF* ptr = (RECV_BUF*) ((char*) b + sizeof(Buffer) + i * sizeof_shm_recv_buf(recv_buf_size));
		shm_recv_buf_init(ptr, recv_buf_size);
	}
}

void Buffer_add(Buffer* b, RECV_BUF* node) {
	if (b == NULL || node == NULL) {
		return;
	}
	/*Buffer full?*/
	if (b->size >= b->max_size) {
		return;
	}
	else if (b->front == -1) {
		b->front = 0;
		b->rear = 0;
		memcpy(b->queue, node, sizeof_shm_recv_buf(10240));
	}
	else if (b->rear == b->max_size - 1 && b->front != 0) {
		b->rear = 0;
		memcpy(b->queue, node, sizeof_shm_recv_buf(10240));
	}
	else {
		b->rear++;
		memcpy(b->queue + b->rear * sizeof_shm_recv_buf(10240), node, sizeof_shm_recv_buf(10240));
	}
	b->size++;
}

void Buffer_pop(Buffer* b) {
	if (b == NULL) {
		return;
	}
	if (b->size == 0) {
	 	return;
	}
	if (b->front == b->rear) {
		b->front = -1;
		b->rear = -1;
	} else if (b->front == b->max_size - 1) {
		b->front = 0;
	} else {
		b->front++;
	}
	b->size--;
}

void Buffer_clean(Buffer *b) {
	if (b->size == 0) return;
	for (int i = 0; i < b->max_size; ++i) {
		shm_recv_buf_cleanup(&b->queue[i]);
	}
	b->front = -1;
	b->rear = -1;
	b->size = 0;
}

int sizeof_Buffer(int buffer_size, size_t recv_buf_size) {
	return (sizeof(Buffer) + sizeof(char) * buffer_size * sizeof_shm_recv_buf(recv_buf_size));
}

/*
int main() {
	Buffer* myBuf = malloc(sizeof(Buffer));
	Buffer_init(myBuf, 5);
	printf("Buffer initial size: %d\n", myBuf->size);
	printf("Buffer max size: %d\n", myBuf->max_size);
	RECV_BUF* testBuf = malloc(sizeof(RECV_BUF));
	Buffer_add(myBuf, testBuf);
	printf("Buffer size: %d\n", myBuf->size);
	Buffer_pop(myBuf);
	printf("Buffer size: %d\n", myBuf->size);
	Buffer_clean(myBuf);
	free(myBuf);
	return 0;
}
*/

