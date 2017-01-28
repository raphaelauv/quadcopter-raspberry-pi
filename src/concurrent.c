#include "concurrent.h"

void init_PMutex(PMutex * arg) {
	if (arg != NULL) {
		arg->mutex = (pthread_mutex_t )PTHREAD_MUTEX_INITIALIZER;
		arg->condition = (pthread_cond_t )PTHREAD_COND_INITIALIZER;
		arg->var = 0;
	}else{
		perror("PMutex not yet malloc\n");
	}
}

void clean_PMutex(PMutex * arg) {
	if (arg != NULL) {
		pthread_mutex_destroy(&arg->mutex);
		pthread_cond_destroy(&arg->condition);
		free(arg);
		arg=NULL;
	}
}


void clean_DataController(DataController * arg){
	if (arg != NULL) {
		clean_PMutex(arg->pmutex);
		free(arg);
		arg=NULL;
	}
}

void sleepDuration(int sleepTime) {
	int result = -1;
	while (result != 0) {
		result = sleep(sleepTime);
		sleepTime -= result;
	}
}