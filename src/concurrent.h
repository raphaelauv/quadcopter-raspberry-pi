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
	float axe_Rotation;
	float axe_UpDown;

	float axe_LeftRight;
	float axe_FrontBack;

	PMutex * pmutex;

}DataController;
void clean_DataController(DataController * arg);

#endif /* CONCURRENT_H_ */
