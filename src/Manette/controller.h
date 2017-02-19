#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "manette.h"

#include "../concurrent.h"

#define Update_Frequence 100000

typedef struct args_CONTROLER {
	char verbose;
	volatile int endController;
	volatile int newThing;
	DataController * manette;
	PMutex * pmutexReadDataController;
	PMutex * pmutexControllerPlug;
} args_CONTROLER;


void *thread_CONTROLER(void *args);

void control( args_CONTROLER * argsControl);
int init_args_CONTROLER(args_CONTROLER ** argControler);
void clean_args_CONTROLER(args_CONTROLER * arg);




#endif /* CONTROLLER_H_ */
