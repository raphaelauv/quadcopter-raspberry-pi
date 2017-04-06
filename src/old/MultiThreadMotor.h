
#ifndef _MultiThreadMotor_H_
#define _MultiThreadMotor_H_

#ifdef __arm__
#include <wiringPi.h>
#endif

#include <stdarg.h>
#include "concurrent.h"

#define NUMBER_OF_MOTORS 4
#define FREQUENCY_MOTOR 50.0

#define TIME_LATENCY 50

#define MOTOR_HIGH_TIME 2000
#define MOTOR_LOW_TIME 1000

#define BROCHE_MOTOR_0 5
#define BROCHE_MOTOR_1 28
#define BROCHE_MOTOR_2 2
#define BROCHE_MOTOR_3 24
/* ... */

#define USEC_TO_SEC 1000000

/**********************************************************************/
/* MULTI THREADING SOLUTION */

typedef struct Motor_info {
	volatile int * bool_arret_moteur; // En cas d'arret d'urgenre =1
	int broche; // nemero de la broche de sorti du signal.
	double high_time; // temps haut du signal
	double low_time; // temps bas du signal.
	PMutex * MutexSetPower; // Mutex pour que set_power() ne modifie pas les valeurs au mauvais moment.
	PMutex * Barrier;
} Motor_info;

typedef struct MotorsAll {
	volatile int * bool_arret_moteur; // TODO volatile
	PMutex * Barrier;
	Motor_info ** arrayOfMotors;
} MotorsAll;


int init_Value_motors(MotorsAll * motorsAll);
int init_MotorsAll(MotorsAll ** motorsAll);

//Initialise les 4 moteur a 0% de puissance(4 thread en RT et sur le coeur 1).
int init_threads_motors(pthread_t * tab,MotorsAll * motorsAll);

void clean_MotorsAll(MotorsAll * arg);

//Change la puissance d'un moteur, power en % (de 0% a 10%),renvoi 1 si echec.
int set_power(Motor_info * info,int high_time);


#endif

/*
EXEMPLE OF USE OF THE API

	MotorsAll * motorsAll;
	if (init_MotorsAll(&motorsAll)) {
		return EXIT_FAILURE;
	}
	
	pthread_t threadMotors[NUMBER_OF_MOTORS];
	
	if(init_threads_motors(threadMotors,motorsAll)){
		return EXIT_FAILURE;
	}
	
	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		if ((pthread_join(threadMotors[i], NULL)) != 0) {
			return EXIT_FAILURE;
		}
	}

	clean_MotorsAll(motorsAll);

*/