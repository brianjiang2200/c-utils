/*Custom Implementation of a fixed size queue to store recieve data from a CURL callback*/
#include <stdio.h>
#include <stdlib.h>
#include "main_write_header_cb.h"
#include "buffer.h"

void Buffer_init(Buffer* b, int max_size) {
	if (b == NULL) {
		b = malloc(sizeof(Buffer));
	}
	b->size = 0;
	b->max_size = max_size;
	b->tail = NULL;
	b->head = NULL;
}

void Buffer_add(Buffer* b, RECV_BUF* node) {
	if (b == NULL || node == NULL) {
		return;
	}
	/*Buffer full?*/
	if (b->size >= b->max_size) {
		return;
	}
	Bnode* new_node = malloc(sizeof(Bnode));
	new_node->buf = node;
	/*general case*/
	if (b->size > 0) {
		b->tail->next = new_node;
		b->tail = new_node;
	}
	/*Buffer empty case*/
	else if (b->size == 0) {
		b->tail = new_node;
		b->head = new_node;
	}
	new_node->next = NULL;
	b->size++;
}

void Buffer_pop(Buffer* b) {
	if (b == NULL) {
		return;
	}
	if (b->size > 0) {
		Bnode* popped = b->tail;
		b->tail = b->tail->next;
		/*deallocate popped*/
		recv_buf_cleanup(popped->buf);
		free(popped->buf);
		free(popped);
		b->size--;
	}
}

void Buffer_clean(Buffer *b) {
	if (b->size > 0) {
		Bnode* stepper = b->tail;
		while (stepper != NULL) {
			Bnode* popped = stepper;
			stepper = stepper->next;
			recv_buf_cleanup(popped->buf);
			free(popped->buf);
			free(popped);
		}
	}
	b->tail = NULL;
	b->head = NULL;
	b->size = 0;
}

int main() {
	/*testing*/
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


