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

/*********************API************************/

#define P 1
#define I 0
#define D 15

#define MAX_CONTROLLER_VALUE 100
#define PID_MAX_ANGLE 35

typedef struct args_PID {
	MotorsAll * motorsAll;
	DataController * dataController;
	RTIMU * imu; //	RTIMU *
	PID_INFO * pidInfo;
} args_PID;

int init_args_PID(args_PID ** argPID);
void clean_args_PID(args_PID * arg);

int start_thread_PID(pthread_t * threadPID,void *threadPID_stack_buf,args_PID * argPID);


/************************************************/

//PID pitch
#define PID_GAIN_P_PITCH P
#define PID_GAIN_I_PITCH I
#define PID_GAIN_D_PITCH D
#define PID_MAX_PITCH 400

//PID roll
#define PID_GAIN_P_ROLL P
#define PID_GAIN_I_ROLL I
#define PID_GAIN_D_ROLL D
#define PID_MAX_ROLL 400

//PID yaw
#define PID_GAIN_P_YAW 3
#define PID_GAIN_I_YAW 0.01
#define PID_MAX_YAW 400


#define PID_ANGLE_PRECISION_MULTIPLE 5
#define PID_ANGLE_MULTIPLE ( MAX_CONTROLLER_VALUE * PID_ANGLE_PRECISION_MULTIPLE ) / PID_MAX_ANGLE

#define VIBRATION_Moving_average 20
#define PID_TIMER_TIME_SECURITE_SECONDE 10
#define PID_TIMER_VERIF_FREQUENCY 20


#endif /* PID_HPP_ */
