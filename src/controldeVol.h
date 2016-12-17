#ifndef CONTROLDEVOL_H_
#define CONTROLDEVOL_H_

#include "concurrent.h"
#include "motors.h"


typedef struct args_CONTROLDEVOL {
	motorsAll * motorsAll;
	boolMutex * mutexReadDataControler;
	struct dataController * dataController;

} args_CONTROLDEVOL;


void * startCONTROLVOL(void * args);

#endif /* CONTROLDEVOL_H_ */
