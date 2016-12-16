#ifndef CONCURRENT_H_
#define CONCURRENT_H_

#include <pthread.h>
#include <stdlib.h>

typedef struct boolMutex {
	int var;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
} boolMutex;

void clean_boolMutex(boolMutex * arg);


#endif /* CONCURRENT_H_ */
