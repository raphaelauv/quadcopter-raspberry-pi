#include "motors.h"

int init_MotorsAll3(MotorsAll3 ** motorsAll3 , volatile sig_atomic_t * boolStopMotor){

	*motorsAll3 = (MotorsAll3 *) malloc(sizeof(MotorsAll3));
	if (*motorsAll3 == NULL) {
		logString("MALLOC FAIL : motorsAll");
		return -1;
	}

	PCA9685 * pcaMotors;
	if (initPCA9685(&pcaMotors, CHANNEL_I2C, 0x40)) {
		logString("initPCA9685 FAIL");
		return -1;
	}

	PMutex * mutexValues = (PMutex *) malloc(sizeof(PMutex));
	if (mutexValues == NULL) {
		logString("MALLOC FAIL : barrier");
		return -1;
	}
	init_PMutex(mutexValues);


	(*motorsAll3)->boolMotorStop=boolStopMotor;
	(*motorsAll3)->MutexSetValues=mutexValues;

	#ifdef __arm__
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

	int result=0;

	pthread_mutex_lock(&MotorsAll3->MutexSetValues->mutex);

	result= (MotorsAll3->motorStop);
	if(result==0){

		for (int i = 0; i < NUMBER_OF_MOTORS; i++) {

				#ifdef __arm__
				PCA9685_setPWM_1(MotorsAll3->motors, i + MINIMUM_LED_VALUE, powers[i]);
				#endif
			}
	}
	pthread_mutex_unlock(&MotorsAll3->MutexSetValues->mutex);

	return result;
}

void setMotorStop(MotorsAll3 * MotorsAll3){

	pthread_mutex_lock(&MotorsAll3->MutexSetValues->mutex);
	MotorsAll3->motorStop=1;
	pthread_mutex_unlock(&MotorsAll3->MutexSetValues->mutex);

	int tabMin[NUMBER_OF_MOTORS];
	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		tabMin[i] = 0;
	}

	set_power3(MotorsAll3,tabMin);
}

int isMotorStop(MotorsAll3 * MotorsAll3){

	//first look to glabal signal value
	int value = *(MotorsAll3->boolMotorStop);
	if(value){
		pthread_mutex_lock(&MotorsAll3->MutexSetValues->mutex);
		MotorsAll3->motorStop=1;
		pthread_mutex_unlock(&MotorsAll3->MutexSetValues->mutex);
		return value;

	//secondly look to the atomic value
	}else{
		pthread_mutex_lock(&MotorsAll3->MutexSetValues->mutex);
		value=MotorsAll3->motorStop;
		pthread_mutex_unlock(&MotorsAll3->MutexSetValues->mutex);
		return value;
	}
}
