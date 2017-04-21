#ifndef PID_HPP_
#define PID_HPP_

#include "concurrent.h"
#include "motors.h"
//#include "RTIMULib/RTIMULib.h"
#include "sensor.hpp"
//#include "Calibrage/Calibration_ESC.h"
#include "ADC/MCP3008.h"

#define PID_SLEEP_TIME_SECURITE 10
#define PID_SLEEP_VERIF_FREQUENCY 20

//PID pitch
#define PID_GAIN_P_PITCH 1.3
#define PID_GAIN_I_PITCH 0.002
#define PID_GAIN_D_PITCH 30
#define PID_MAX_PITCH 400
/*1.5, 0.002 220*/

//PID roll
#define PID_GAIN_P_ROLL 1.3
#define PID_GAIN_I_ROLL 0.002
#define PID_GAIN_D_ROLL 30
#define PID_MAX_ROLL 400

//PID yaw
#define PID_GAIN_P_YAW 1.3
#define PID_GAIN_I_YAW 0.002
#define PID_MAX_YAW 400

#define MAX_CONTROLLER_VALUE 100
#define PID_MAX_ANGLE 35
#define PID_ANGLE_PRECISION_MULTIPLE 5

#define PID_ANGLE_MULTIPLE ( MAX_CONTROLLER_VALUE * PID_ANGLE_PRECISION_MULTIPLE ) / PID_MAX_ANGLE

typedef struct args_PID {
	MotorsAll3 * motorsAll3;
	DataController * dataController;
	RTIMU * imu; //	RTIMU *

} args_PID;

int init_args_PID(args_PID ** argPID,DataController * dataControl,MotorsAll3 * motorsAll3);
int init_thread_PID(pthread_t * threadPID,void *threadPID_stack_buf,args_PID * argPID);
void clean_args_PID(args_PID * arg);

#endif /* PID_HPP_ */
