#ifndef PID_HPP_
#define PID_HPP_

#include "concurrent.h"
#include "motors.h"
#include "RTIMULib/RTIMULib.h"
//#include "capteur.hpp"
//#include "Calibrage/Calibration_ESC.h"


typedef struct args_PID {
	char verbose;
	MotorsAll * motorsAll;
	DataController * dataController;
	RTIMU * imu; //	RTIMU *

} args_PID;

int init_args_PID(args_PID ** argPID,DataController * dataControl,MotorsAll * motorsAll);

int init_thread_PID(pthread_t * threadControlerVOL,args_PID * argPID);

void clean_args_PID(args_PID * arg);

#endif /* PID_HPP_ */
