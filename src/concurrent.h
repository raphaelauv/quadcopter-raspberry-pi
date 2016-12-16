#ifndef CONCURRENT_H_
#define CONCURRENT_H_

#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

typedef struct dataController{
	char moteur_active;
	float moteur0;
	float moteur1;
	float moteur2;
	float moteur3;
}dataController;

typedef struct boolMutex {
	int var;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
} boolMutex;

void init_boolMutex(boolMutex * arg);

void clean_boolMutex(boolMutex * arg);

#endif /* CONCURRENT_H_ */
