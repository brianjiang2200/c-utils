/*Custom Implementation of a fixed size queue to store recieve data from a CURL callback*/
#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"

void Buffer_init(Buffer* b, int max_size) {
	if (b == NULL) {
		b = (Buffer*) malloc(sizeof(Buffer));
	}
	b->size = 0;
	b->max_size = max_size;
	b->front = -1;
	b->rear = -1;

	//b->q_shmid will be used to reference the specific queue (which image seg) we need
	b->q_shmid = smhget(IPC_PRIVATE, b->max_size * sizeof(RECV_BUF), 0666 | IPC_CREAT);
	b->queue = (RECV_BUF*) shmat(b->q_shmid, NULL, 0);
	for (int i = 0; i < max_size; ++i) {
		recv_buf_init(&b->queue[i], 10000);
	}
	/*do not detach yet*/
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
		memcpy(&b->queue[b->rear], node, sizeof(RECV_BUF));
		memcpy(b->queue[b->rear].buf, node->buf, node->size);
	}
	else if (b->rear == b->max_size - 1 && b->front != 0) {
		b->rear = 0;
		memcpy(&b->queue[b->rear], node, sizeof(RECV_BUF));
		memcpy(b->queue[b->rear].buf, node->buf, node->size);
	}
	else {
		b->rear++;
		memcpy(&b->queue[b->rear], node, sizeof(RECV_BUF));
		memcpy(b->queue[b->rear].buf, node->buf, node->size);
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
		recv_buf_cleanup(&b->queue[i]);
	}
	b->front = -1;
	b->rear = -1;
	b->size = 0;
	/*detach and destroy*/
	if (shmdt(b->queue) != 0) {
		perror("shmdt");
		abort();
	}
	if (shmctl(b->q_shmid, IPC_RMID, NULL) == -1) {
		perror("shmctl");
		abort();
	}
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

