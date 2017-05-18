#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "joystick.h"

#define CONTROLLER_NUMBER_AXES 4

#define CONTROLLER_LIMIT_PRECISION 20				// Values under 20 and -20 on the remote are not evaluate
#define CONTROLLER_FLANGE 8							// Higher is the value higher is the flange

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
