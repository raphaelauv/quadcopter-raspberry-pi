#ifndef CONCURRENT_H_
#define CONCURRENT_H_

#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>


typedef struct PMutex {
	int var;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
} PMutex;

void init_PMutex(PMutex * arg);
void clean_PMutex(PMutex * arg);


typedef struct DataController{


	char moteur_active;
	float moteur0;
	float moteur1;

	float moteur2;
	float moteur3;

	PMutex * pmutex;

}DataController;
void clean_DataController(DataController * arg);

#endif /* CONCURRENT_H_ */
