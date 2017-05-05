#ifndef PID_HPP_
#define PID_HPP_

#include <math.h>
#include "concurrent.h"
#include "motors.h"
//#include "old/MonoThreadMotor.h"
#include "sensor.hpp"
#include "ADC/MCP3008.h"

//#include "RTIMULib/RTIMULib.h"
//#include "Calibrage/Calibration_ESC.h"


#define PID_SLEEP_TIME_SECURITE 10
#define PID_SLEEP_VERIF_FREQUENCY 20

#define P 1.8
#define I 0.01
#define D 80

//PID pitch
#define PID_GAIN_P_PITCH P
#define PID_GAIN_I_PITCH I
#define PID_GAIN_D_PITCH D
#define PID_MAX_PITCH 400
/*1.5, 0.002 220*/

//PID roll
#define PID_GAIN_P_ROLL P
#define PID_GAIN_I_ROLL I
#define PID_GAIN_D_ROLL D
#define PID_MAX_ROLL 400

//PID yaw
#define PID_GAIN_P_YAW 5
#define PID_GAIN_I_YAW 0.02
#define PID_MAX_YAW 400

#define MAX_CONTROLLER_VALUE 100
#define PID_MAX_ANGLE 35
#define PID_ANGLE_PRECISION_MULTIPLE 5

#define PID_ANGLE_MULTIPLE ( MAX_CONTROLLER_VALUE * PID_ANGLE_PRECISION_MULTIPLE ) / PID_MAX_ANGLE

typedef struct args_PID {
	MotorsAll * motorsAll3;
	DataController * dataController;
	RTIMU * imu; //	RTIMU *
	PID_INFO * pidInfo;
} args_PID;

int init_args_PID(args_PID ** argPID);
int init_thread_PID(pthread_t * threadPID,void *threadPID_stack_buf,args_PID * argPID);
void clean_args_PID(args_PID * arg);

#endif /* PID_HPP_ */
