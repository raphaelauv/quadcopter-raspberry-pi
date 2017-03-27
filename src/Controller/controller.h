#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "manette.h"

#include "../concurrent.h"

#define XBOX_CONTROLLER_MAX_VALUE 32768

typedef struct args_CONTROLLER {
	char verbose;
	volatile int endController;
	volatile int newThing;
	DataController * manette;
	PMutex * pmutexReadDataController;
	PMutex * pmutexControllerPlug;
} args_CONTROLLER;


void *thread_CONTROLLER(void *args);

void control( args_CONTROLLER * argsControl);
int init_args_CONTROLLER(args_CONTROLLER ** argControler);
void clean_args_CONTROLLER(args_CONTROLLER * arg);




#endif /* CONTROLLER_H_ */
