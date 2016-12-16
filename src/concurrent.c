#include "concurrent.h"

void init_boolMutex(boolMutex * arg) {
	if (arg != NULL) {
		arg->mutex = (pthread_mutex_t )PTHREAD_MUTEX_INITIALIZER;
		arg->condition = (pthread_cond_t )PTHREAD_COND_INITIALIZER;
		arg->var = 0;
	}
}

void clean_boolMutex(boolMutex * arg) {
	if (arg != NULL) {
		pthread_mutex_destroy(&arg->mutex);
		pthread_cond_destroy(&arg->condition);
		free(arg);
	}
}
