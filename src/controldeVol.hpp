#ifndef CONTROLDEVOL_HPP_
#define CONTROLDEVOL_HPP_

#include "concurrent.h"
#include "motors.h"
#include "RTIMULib/RTIMULib.h"
//#include "capteur.hpp"
//#include "Calibrage/Calibration_ESC.h"


typedef struct args_CONTROLDEVOL {
	char verbose;
	MotorsAll * motorsAll;
	DataController * dataController;
	RTIMU * imu; //	RTIMU *

} args_CONTROLDEVOL;

int init_thread_PID(pthread_t * threadControlerVOL,args_CONTROLDEVOL * argCONTROLVOL);

void clean_args_CONTROLDEVOL(args_CONTROLDEVOL * arg);

#endif /* CONTROLDEVOL_HPP_ */
