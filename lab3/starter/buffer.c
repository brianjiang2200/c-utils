/*Custom Implementation of a fixed size queue to store recieve data from a CURL callback*/
#include <stdio.h>
#include <stdlib.h>
#include "main_write_header_cb.h"

typedef struct Buffer
{
	int size;
	RECV_BUF* tail;
	RECV_BUF* head;
} Buffer;


