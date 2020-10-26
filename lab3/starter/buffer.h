#include <stdio.h>
#include <stdlib.h>
#include "main_write_header_cb.h"

typedef struct Buffer
{
	int size;
	RECV_BUF* tail;
	RECV_BUF* head;
} Buffer;

void Buffer_init();
void Buffer_add(RECV_BUF* node);
void Buffer_pop();



