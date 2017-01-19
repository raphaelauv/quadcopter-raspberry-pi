#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "manette.h"

#include "../concurrent.h"

#define Update_Frequence 1


typedef struct args_CONTROLER {
	int newThing;
	DataController * manette;
	PMutex * pmutexReadDataController;
	PMutex * pmutexControlerPlug;
} args_CONTROLER;


void control( args_CONTROLER * argsControl);


#endif /* CONTROLLER_H_ */
