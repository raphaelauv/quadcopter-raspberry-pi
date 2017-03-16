/*
 * Les motors son controle par des ESCs qui prenne en entré un signal carré de 50hz(Par default)
 * On controle donc le rapport cyclique de ce signal pour controlé la vitesse du drone.
 * 1 milliseconde -> 0% de puissance
 * 2 milliseconde -> 100% de puissance
 * */

#ifndef _MOTORS_H_
#define _MOTORS_H_

#ifdef __arm__
#include <wiringPi.h>
#endif

#include <stdarg.h>
#include "concurrent.h"

#define NUMBER_OF_MOTORS 4
#define FREQUENCY_MOTOR 50.0

#define TIME_LATENCY 5

#define MOTOR_HIGH_TIME 2000
#define MOTOR_LOW_TIME 1000

#define BROCHE_MOTOR_0 5 //Up right
#define BROCHE_MOTOR_1 28 //Down right
#define BROCHE_MOTOR_2 2 //Down left
#define BROCHE_MOTOR_3 24 //Up left
#define BROCHE_MOTOR_4 0
#define BROCHE_MOTOR_5 0
#define BROCHE_MOTOR_6 0
#define BROCHE_MOTOR_7 0
/* ... */


/**********************************************************************/
/* ONE THREAD SOLUTION */

typedef struct MotorsAll2 {
	volatile int * bool_arret_moteur;
	PMutex * MutexSetValues;
	int broche[NUMBER_OF_MOTORS];
	int high_time[NUMBER_OF_MOTORS];
} MotorsAll2;

int init_MotorsAll2(MotorsAll2 ** motorsAll2);
void clean_MotorsAll2(MotorsAll2 * arg);
int init_thread_startMotorAll2(pthread_t * pthread,MotorsAll2 * MotorsAll2);
int set_power2(MotorsAll2 * MotorsAll2, int * powers);
void setMotorStop(MotorsAll2 * MotorsAll2);
int isMotorStop(MotorsAll2 * MotorsAll2);

#endif
