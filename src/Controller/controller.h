#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "joystick.h"
#include "../concurrent.h"

#define CONTROLLER_NUMBER_AXES 4
#define CONTROLLER_LIMIT_PRECISION 10				// Values under k and -k on the remote are not evaluate
#define CONTROLLER_FLANGE 8							// Higher is the value, higher is the flange

#define CONTROLLER_LIMIT 50

#define CONTROLLER_MAX 100

#define CONTROLLER_UP_DOWN_VARIABLE_MODE 1

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
