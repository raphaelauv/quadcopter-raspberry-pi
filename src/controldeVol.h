#ifndef CONTROLDEVOL_H_
#define CONTROLDEVOL_H_

#include "concurrent.h"
#include "motors.h"


typedef struct args_CONTROLDEVOL {
	MotorsAll * motorsAll;
	DataController * dataController;

} args_CONTROLDEVOL;


void * startCONTROLVOL(void * args);

void clean_args_CONTROLDEVOL(args_CONTROLDEVOL * arg);

#endif /* CONTROLDEVOL_H_ */
