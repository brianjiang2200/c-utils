#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "main_2proc.h"

typedef struct Buffer
{
	int size;
	int max_size;
	int front;
	int rear;
	RECV_BUF* queue;
} Buffer;

void Buffer_init(Buffer* b, int max_size);
void Buffer_add(Buffer* b, RECV_BUF* node);
void Buffer_pop(Buffer* b);
void Buffer_clean(Buffer *b);
int sizeof_Buffer(size_t recv_buf_size);



