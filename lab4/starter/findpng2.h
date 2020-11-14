#pragma once

#include <stdio.h>
#include <stdlib.h>

/*nodes in frontier linked list*/
typedef struct dummy1
{
	char* url;
	struct dummy1* next;
} frontier_node;

/*PNG Node*/
typedef struct dummy2
{
	char* url;
	struct dummy2* next;
} png_node;

typedef struct dummmy3
{
	frontier_node* fhead;
	frontier_node* ftail;
	png_node* phead;
	int* pngs_collected;
	int target;
	char* logfile;
	pthread_cond_t* sig_frontier;
	pthread_mutex_t* mut_frontier;
	pthread_mutex_t* mut_pngs;
	pthread_rwlock_t* rw_hash;
	pthread_mutex_t* mut_log;
} thread_args;
