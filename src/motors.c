#include "motors.h"

int init_MotorsAll3(MotorsAll3 ** motorsAll3){

	*motorsAll3 = (MotorsAll3 *) malloc(sizeof(MotorsAll3));
	if (*motorsAll3 == NULL) {
		logString("MALLOC FAIL : motorsAll");
		return -1;
	}

	PCA9685 * pcaMotors;
	if (initPCA9685(&pcaMotors, 0, 0x40)) {
		logString("initPCA9685 FAIL");
		return -1;
	}

	PMutex * mutexValues = (PMutex *) malloc(sizeof(PMutex));
	if (mutexValues == NULL) {
		logString("MALLOC FAIL : barrier");
		return -1;
	}
	init_PMutex(mutexValues);

	(*motorsAll3)->MutexSetValues=mutexValues;

	(*motorsAll3)->motorStop=0;

	#ifdef __arm__
	PCA9685_setPWMFreq(pcaMotors,FREQUENCY_MOTOR);
	PCA9685_setPWM_1(pcaMotors,1, MOTOR_LOW_TIME);
	PCA9685_setPWM_1(pcaMotors,2, MOTOR_LOW_TIME);
	PCA9685_setPWM_1(pcaMotors,3, MOTOR_LOW_TIME);
	PCA9685_setPWM_1(pcaMotors,4, MOTOR_LOW_TIME);
	#endif

	(*motorsAll3)->motors=pcaMotors;

	return 0;

}


void clean_MotorsAll3(MotorsAll3 * arg) {
	if (arg != NULL) {
		clean_PMutex(arg->MutexSetValues);
		cleanPCA9685(arg->motors);
		free(arg);
		arg = NULL;
	}
}

int set_power3(MotorsAll3 * MotorsAll3, int * powers){

	pthread_mutex_lock(&MotorsAll3->MutexSetValues->mutex);

	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		#ifdef __arm__
		PCA9685_setPWM_1(MotorsAll3->motors, i, powers[i]);
		#endif
	}

	pthread_mutex_unlock(&MotorsAll3->MutexSetValues->mutex);
}

void setMotorStop(MotorsAll3 * MotorsAll3){

	pthread_mutex_lock(&MotorsAll3->MutexSetValues->mutex);
	MotorsAll3->motorStop=1;
	pthread_mutex_unlock(&MotorsAll3->MutexSetValues->mutex);
}

int isMotorStop(MotorsAll3 * MotorsAll3){

	int value;
	pthread_mutex_lock(&MotorsAll3->MutexSetValues->mutex);
	value= MotorsAll3->motorStop;
	pthread_mutex_unlock(&MotorsAll3->MutexSetValues->mutex);
	return value;
}
