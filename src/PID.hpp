#ifndef PID_HPP_
#define PID_HPP_

#include "concurrent.h"
#include "motors.h"
#include "RTIMULib/RTIMULib.h"
#include "capteur.hpp"
//#include "Calibrage/Calibration_ESC.h"

#define  PID_SLEEP_TIME_SECURITE 10


//PID pitch
#define  PID_GAIN_P_PITCH 2.5
#define  PID_GAIN_I_PITCH 0.005
#define  PID_GAIN_D_PITCH 220
#define  PID_MAX_PITCH 400
/*2.5, 0.005 220*/

typedef struct args_PID {
	MotorsAll2 * motorsAll2;
	DataController * dataController;
	RTIMU * imu; //	RTIMU *

} args_PID;

int init_args_PID(args_PID ** argPID,DataController * dataControl,MotorsAll2 * motorsAll);

int init_thread_PID(pthread_t * threadControlerVOL,args_PID * argPID);

void clean_args_PID(args_PID * arg);

#endif /* PID_HPP_ */
