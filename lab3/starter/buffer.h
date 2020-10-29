#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "main_write_header_cb.h"

typedef struct Bnode_tmp
{
	RECV_BUF* buf;
	struct Bnode_tmp* next;
} Bnode;

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



