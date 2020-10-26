#include <stdio.h>
#include <stdlib.h>
#include "main_write_header_cb.h"

typedef struct Bnode
{
	RECV_BUF* buf;
	Bnode* next;
} Bnode;

typedef struct Buffer
{
	int size;
	int max_size;
	Bnode* tail;
	Bnode* head;
} Buffer;

void Buffer_init(Buffer* b, int max_size);
void Buffer_add(Buffer* b, RECV_BUF* node);
void Buffer_pop(Buffer* b);
void Buffer_clean(Buffer *b);



