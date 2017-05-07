#ifndef _MOTORS_H_
#define _MOTORS_H_

#include "concurrent.h"
#include "PWM/PCA9685.h"

#define MOTOR_LOW_TIME 1000
#define MOTOR_HIGH_TIME 2000
#define MOTOR_MIN_ROTATE_TIME 1050
#define NUMBER_OF_MOTORS 4


typedef struct MotorsAll {
	PCA9685 * motors;
	PMutex * MutexSetValues;
	int motorStop;
	volatile sig_atomic_t * boolMotorStop;
} MotorsAll;

int init_MotorsAll(MotorsAll ** motorsAll,volatile sig_atomic_t * boolMotorStop);
int set_power(MotorsAll * MotorsAll, int * powers);
void set_Motor_Stop(MotorsAll * MotorsAll);
int is_Motor_Stop(MotorsAll * MotorsAll);
void clean_MotorsAll(MotorsAll * arg);

#endif
