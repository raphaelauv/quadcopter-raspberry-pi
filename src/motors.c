#include "motors.h"

int init_MotorsAll(MotorsAll ** motorsAll , volatile sig_atomic_t * boolStopMotor){

	int testError=0;
	PCA9685 * pcaMotors;
	PMutex * mutexValues;

	*motorsAll = (MotorsAll *) malloc(sizeof(MotorsAll));
	if (*motorsAll == NULL) {
		logString("MALLOC FAIL : init_MotorsAll motorsAll");
		goto cleanFail;
	}

	setMode_PCA9685(FLAG_CUSTOM_I2C);

	if (initPCA9685(&pcaMotors, CHANNEL_I2C, 0x40)) {
		logString("initPCA9685 FAIL");
		goto cleanFail;
	}
	(*motorsAll)->motors=pcaMotors;

	mutexValues = (PMutex *) malloc(sizeof(PMutex));
	if (mutexValues == NULL) {
		logString("MALLOC FAIL : init_MotorsAll mutexValues");
		goto cleanFail;
	}
	(*motorsAll)->MutexSetValues=mutexValues;
	init_PMutex(mutexValues);


	(*motorsAll)->boolMotorStop=boolStopMotor;
	(*motorsAll)->motorStop=0;


	#ifdef __arm__
	testError+=PCA9685_setPWM_1(pcaMotors,1, MOTOR_LOW_TIME);
	testError+=PCA9685_setPWM_1(pcaMotors,2, MOTOR_LOW_TIME);
	testError+=PCA9685_setPWM_1(pcaMotors,3, MOTOR_LOW_TIME);
	testError+=PCA9685_setPWM_1(pcaMotors,4, MOTOR_LOW_TIME);
	#endif

	if(testError){
		logString("MALLOC FAIL : init_MotorsAll PCA9685_setPWM_1");
		goto cleanFail;
	}

	return 0;

cleanFail:
	clean_MotorsAll(*motorsAll);
	return -1;

}


void clean_MotorsAll(MotorsAll * arg) {
	if (arg != NULL) {
		cleanPCA9685(arg->motors);
		clean_PMutex(arg->MutexSetValues);
		free(arg);
	}
}

int set_power(MotorsAll * MotorsAll, int * powers){

	int result=0;

	pthread_mutex_lock(&MotorsAll->MutexSetValues->mutex);

	result= (MotorsAll->motorStop);

	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {

		#ifdef __arm__
		if(result==0){
			PCA9685_setPWM_1(MotorsAll->motors, i + MINIMUM_LED_VALUE, powers[i]);
		}else{
			PCA9685_setPWM_1(MotorsAll->motors, i + MINIMUM_LED_VALUE, 0);
		}
		#endif
	}

	pthread_mutex_unlock(&MotorsAll->MutexSetValues->mutex);

	return result;
}

void set_Motor_Stop(MotorsAll * MotorsAll){

	pthread_mutex_lock(&MotorsAll->MutexSetValues->mutex);
	MotorsAll->motorStop=1;
	pthread_mutex_unlock(&MotorsAll->MutexSetValues->mutex);
	set_power(MotorsAll,NULL);


	//TODO -> to delete
	/* 
	int tabMin[NUMBER_OF_MOTORS];
	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		tabMin[i] = MOTOR_LOW_TIME;
	}

	set_power3(MotorsAll3,tabMin);
	*/
	
}

int is_Motor_Stop(MotorsAll * MotorsAll){

	//first look to glabal signal value
	int value = *(MotorsAll->boolMotorStop);
	if(value){
		pthread_mutex_lock(&MotorsAll->MutexSetValues->mutex);
		MotorsAll->motorStop=1;
		pthread_mutex_unlock(&MotorsAll->MutexSetValues->mutex);
		return value;

	//or look to the atomic value
	}else{
		pthread_mutex_lock(&MotorsAll->MutexSetValues->mutex);
		value=MotorsAll->motorStop;
		pthread_mutex_unlock(&MotorsAll->MutexSetValues->mutex);
		return value;
	}
}
