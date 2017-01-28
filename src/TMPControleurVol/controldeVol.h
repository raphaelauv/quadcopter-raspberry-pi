#ifndef CONTROLDEVOL_H_
#define CONTROLDEVOL_H_

#include "concurrent.h"
#include "motors.h"
#include "RTIMULib.h"
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>

#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/mman.h>
//pid roll
#define  PID_GAIN_P_ROLL 1
#define  PID_GAIN_I_ROLL 0
#define  PID_GAIN_D_ROLL 0
#define  PID_MAX_ROLL 400
//PID pitch
#define  PID_GAIN_P_PITCH (PID_GAIN_P_ROLL)
#define  PID_GAIN_I_PITCH (PID_GAIN_I_ROLL)
#define  PID_GAIN_D_PITCH (PID_GAIN_D_ROLL)
#define  PID_MAX_PITCH (PID_MAX_ROLL)
//PID yaw
#define PID_GAIN_P_YAW 1
#define PID_GAIN_I_YAW 0
#define PID_GAIN_D_YAW 0
#define PID_MAX_YAW 400

typedef struct args_CONTROLDEVOL {
	MotorsAll * motorsAll;
	DataController * dataController;

} args_CONTROLDEVOL;

void init_threads_controle(args_CONTROLDEVOL  * controle_vol);
void * startCONTROLVOL(void * args);

void clean_args_CONTROLDEVOL(args_CONTROLDEVOL * arg);

#endif /* CONTROLDEVOL_H_ */
