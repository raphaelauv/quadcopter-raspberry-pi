#include "serv.h"
#include "motors.h"
#include "PID.hpp"
#include "capteur.hpp"

void stopNetwork(volatile int * boolStopServ){
	*boolStopServ=1;
	sleep(3);
}

int main (int argc, char *argv[]){

	char noControl=0;
	if(setVerboseOrLog(argc,argv[1],1)){
		return EXIT_FAILURE;
	}
	setNoControl(&noControl,argc,argv[2],2);

	args_SERVER * argServ;
	if(init_args_SERVER(&argServ)){
		return EXIT_FAILURE;
	}

	/*
	MotorsAll * motorsAll;
	if (init_MotorsAll(&motorsAll)) {
		return EXIT_FAILURE;
	}
	*/

	MotorsAll2 * motorsAll2;
	if (init_MotorsAll2(&motorsAll2,4,BROCHE_MOTOR_0,BROCHE_MOTOR_1,BROCHE_MOTOR_2,BROCHE_MOTOR_3)) {
		return EXIT_FAILURE;
	}

	args_PID * argPID;
	if (init_args_PID(&argPID,argServ->dataController,motorsAll2)) {
		return EXIT_FAILURE;
	}

	#ifdef __arm__
	RTIMU *imu;
	imu = capteurInit();

	if(imu==NULL){
		logString("THREAD MAIN : ERROR NEW FAIL RTIMU ->imu");
		return EXIT_FAILURE;
	}else{
		logString("THREAD MAIN : CAPTEUR INIT SUCCES");
		argPID->imu=imu;
	}
	#endif

	pthread_t threadServer;
	pthread_t threadPID;
	pthread_t threadMotor2;
	//pthread_t threadMotors[NUMBER_OF_MOTORS];

	if(!noControl){
		pthread_mutex_lock(&argServ->pmutexRemoteConnect->mutex);

		if (pthread_create(&threadServer, NULL, thread_UDP_SERVER, argServ)) {
			logString("THREAD MAIN : pthread_create SERVER\n");
			return EXIT_FAILURE;
		}

		pthread_cond_wait(&argServ->pmutexRemoteConnect->condition, &argServ->pmutexRemoteConnect->mutex);

		pthread_mutex_unlock(&argServ->pmutexRemoteConnect->mutex);
	}


	if(init_thread_PID(&threadPID,argPID)){
		stopNetwork(argServ->boolStopServ);
		return EXIT_FAILURE;
	}


	if(init_thread_startMotorAll2(&threadMotor2,motorsAll2)){
		stopNetwork(argServ->boolStopServ);
		return EXIT_FAILURE;
	}


	/*
	if(init_threads_motors(threadMotors,motorsAll)){
		*argServ->boolStopServ=1;
		//todo ask for PID close
		return EXIT_FAILURE;
	}
	*/
	int * returnValue;

	if (pthread_join(threadPID, (void**) &returnValue)){
		logString("THREAD MAIN : ERROR pthread_join PID");
		stopNetwork(argServ->boolStopServ);
		return EXIT_FAILURE;
	}

	if (pthread_join(threadMotor2, (void**) &returnValue)) {
		logString("THREAD MAIN : ERROR pthread_join MOTOR");
		stopNetwork(argServ->boolStopServ);
		return EXIT_FAILURE;
	}

	if (pthread_join(threadServer,(void**) &returnValue)){
		logString("THREAD MAIN : ERROR pthread_join SERVER");
		return EXIT_FAILURE;
	}


	/*
	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		if ((pthread_join(threadMotors[i], NULL)) != 0) {
			logString("THREAD MAIN : ERROR pthread_join MOTOR");
			return EXIT_FAILURE;
		}
	}
	*/

	clean_args_SERVER(argServ);
	clean_args_PID(argPID);

	logString("THREAD MAIN : END");
	closeLogFile();
	return EXIT_SUCCESS;
}
