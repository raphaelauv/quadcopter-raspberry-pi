#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "manette.h"

#include "../concurrent.h"

#define XBOX_CONTROLLER_MAX_VALUE 32768

typedef struct args_CONTROLLER {
	char verbose;
	int controllerStop;
	volatile int newThing;
	DataController * manette;
	PMutex * pmutexReadDataController;
	PMutex * pmutexControllerPlug;
	volatile sig_atomic_t * boolStopController;
} args_CONTROLLER;



void set_Controller_Stop(args_CONTROLLER * argControler);
int is_Controller_Stop(args_CONTROLLER * argControler);


void *thread_CONTROLLER(void *args);
int init_args_CONTROLLER(args_CONTROLLER ** argControler,volatile sig_atomic_t * boolStopController);
void clean_args_CONTROLLER(args_CONTROLLER * arg);




#endif /* CONTROLLER_H_ */
