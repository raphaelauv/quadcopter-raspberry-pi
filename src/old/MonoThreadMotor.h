/*
 * Les motors son controle par des ESCs qui prenne en entré un signal carré de 50hz(Par default)
 * On controle donc le rapport cyclique de ce signal pour controlé la vitesse du drone.
 * 1 milliseconde -> 0% de puissance
 * 2 milliseconde -> 100% de puissance
 * */

#ifndef _MonoThreadMotor_H_
#define _MonoThreadMotor_H_

#ifdef __arm__
#include <wiringPi.h>
#endif

#include "../concurrent.h"

#define TIME_LATENCY 5

#define MOTOR_LOW_TIME 1000
#define MOTOR_HIGH_TIME 2000

#define MOTOR_MIN_ROTATE_TIME 1050

#define NUMBER_OF_MOTORS 4

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

typedef struct MotorsAll {
	volatile sig_atomic_t * boolMotorStop;
	PMutex * MutexSetValues;
	int motorStop;
	int broche[NUMBER_OF_MOTORS];
	int high_time[NUMBER_OF_MOTORS];
} MotorsAll;


int init_MotorsAll(MotorsAll ** motorsAll2,volatile sig_atomic_t * boolMotorStop);
int init_thread_startMotorAll(pthread_t * pthread,void * threadMotor2_stack_buf,MotorsAll * motorsAll2);
int set_power(MotorsAll * MotorsAll2, int * powers);
void set_Motor_Stop(MotorsAll * MotorsAll2);
int is_Motor_Stop(MotorsAll * MotorsAll2);
void clean_MotorsAll(MotorsAll * arg);

#endif


/*
 Exemple of Use of API motor2

 MotorsAll * motorsAll2;
 if (init_MotorsAll(&motorsAll2)) {
 return EXIT_FAILURE;
 }

 void *threadMotor2_stack_buf=NULL;

 if(init_thread_startMotorAll(&threadMotor2,threadMotor2_stack_buf,motorsAll2)){
 drone_stopAll();
 return EXIT_FAILURE;
 }

 if (pthread_join(threadMotor2, (void**) &returnValue)) {
 logString("THREAD MAIN : ERROR pthread_join MOTOR");
 drone_stopAll();
 return EXIT_FAILURE;
 }

 if(threadMotor2_stack_buf){
 munmap(threadMotor2_stack_buf, PTHREAD_STACK_MIN);
 }

 */
