
//
//  Test_PID.h
//  Drone
//
//  Created by andres quiroz on 13/02/2017.
//  Copyright © 2017 andres quiroz. All rights reserved.
//

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

//PID pitch
#define  PID_GAIN_P_PITCH 2.5
#define  PID_GAIN_I_PITCH 0.005
#define  PID_GAIN_D_PITCH 220
#define  PID_MAX_PITCH 400
/*2.5, 0.005 220*/

typedef struct args_CONTROLDEVOL {
    MotorsAll * motorsAll;
    DataController * dataController;
    
} args_CONTROLDEVOL;

void init_threads_controle(args_CONTROLDEVOL  * controle_vol);
void * startCONTROLVOL(void * args);

void clean_args_CONTROLDEVOL(args_CONTROLDEVOL * arg);

#endif /* CONTROLDEVOL_H_ */