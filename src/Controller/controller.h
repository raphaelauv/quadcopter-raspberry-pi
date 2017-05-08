#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "manette.h"
#include "../concurrent.h"

#define XBOX_CONTROLLER_MAX_VALUE 32768

typedef struct args_CONTROLLER {

	volatile sig_atomic_t * signalControllerStop;
	int controllerStop;
	PMutex * pmutex;

	PMutex * pmutexControllerPlug;
	DataController * dataControl;


} args_CONTROLLER;



void set_Controller_Stop(args_CONTROLLER * argControler);
int is_Controller_Stop(args_CONTROLLER * argControler);


void *thread_CONTROLLER(void *args);
int init_args_CONTROLLER(args_CONTROLLER ** argControler,volatile sig_atomic_t * signalControllerStop);
void clean_args_CONTROLLER(args_CONTROLLER * arg);


#endif /* CONTROLLER_H_ */
