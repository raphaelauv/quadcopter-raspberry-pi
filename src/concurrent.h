#ifndef CONCURRENT_H_
#define CONCURRENT_H_

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <string.h>

typedef struct PMutex {
	int var;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
} PMutex;

void init_PMutex(PMutex * arg);
void clean_PMutex(PMutex * arg);


typedef struct DataController{

	//FLAG 0-> ARRET | 1 -> PAUSE | 2 -> NORMAL | 3 -> ...
	char flag;
	float axe_Rotation;
	float axe_UpDown;

	float axe_LeftRight;
	float axe_FrontBack;

	PMutex * pmutex;

}DataController;

void clean_DataController(DataController * arg);

void sleepDuration(int sleepTime);
void UsleepDuration(int sleepTime);

int init_Attr_Pthread(pthread_attr_t *attributs, int priority,int id_cpu);

void setVerbose(char * verbose, int argc, char * argv,int min);
/*
#ifdef __cplusplus
}
#endif
*/
#endif /* CONCURRENT_H_ */
