#include "serv.h"
#include "motors.h"
#include "controldeVol.hpp"
#include "capteur.hpp"


int main (int argc, char *argv[]){

	char verbose=0;
	char noControl=0;
	if(setVerboseOrLog(&verbose,argc,argv[1],1)){
		return EXIT_FAILURE;
	}
	setNoControl(&noControl,argc,argv[2],2);

	args_SERVER * argServ;
	if(init_args_SERVER(&argServ)){
		return EXIT_FAILURE;
	}

	MotorsAll * motorsAll;
	if (init_MotorsAll(&motorsAll)) {
		return EXIT_FAILURE;
	}

	args_CONTROLDEVOL * argCONTROLVOL;
	if (init_args_CONTROLDEVOL(&argCONTROLVOL,argServ->dataController,motorsAll)) {
		return EXIT_FAILURE;
	}

	#ifdef __arm__
	RTIMU *imu;
	imu = capteurInit();

	if(imu==NULL){
		logString("NEW FAIL : RTIMU ->imu");
		return EXIT_FAILURE;
	}else{
		logString("THREAD MAIN : CAPTEUR INIT SUCCES\n");
		argCONTROLVOL->imu=imu;
	}
	#endif

	pthread_t threadServer;
	pthread_t threadPID;
	pthread_t threadMotors[NUMBER_OF_MOTORS];

	if(!noControl){
		pthread_mutex_lock(&argServ->pmutexRemoteConnect->mutex);

		if (pthread_create(&threadServer, NULL, thread_UDP_SERVER, argServ)) {
			logString("THREAD MAIN : pthread_create SERVER\n");
			return EXIT_FAILURE;
		}

		pthread_cond_wait(&argServ->pmutexRemoteConnect->condition, &argServ->pmutexRemoteConnect->mutex);

		pthread_mutex_unlock(&argServ->pmutexRemoteConnect->mutex);
	}


	if(init_thread_PID(&threadPID,argCONTROLVOL)){
		*argServ->boolStopServ=1;
		return EXIT_FAILURE;
	}

	if(init_threads_motors(threadMotors,motorsAll)){
		*argServ->boolStopServ=1;
		//todo ask for PID close
		return EXIT_FAILURE;
	}

	int * returnValue;

	if (pthread_join(threadPID, (void**) &returnValue)){
			logString("THREAD MAIN : pthread_join PID");
			return EXIT_FAILURE;
	}

	if (pthread_join(threadServer,(void**) &returnValue)){
		logString("THREAD MAIN : pthread_join SERVER");
		return EXIT_FAILURE;
	}


	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		if ((pthread_join(threadMotors[i], NULL)) != 0) {
			logString("FAIL pthread_join MOTOR ");
			return EXIT_FAILURE;
		}
	}


	clean_args_SERVER(argServ);
	clean_args_CONTROLDEVOL(argCONTROLVOL);

	logString("THREAD MAIN : END");
	closeLogFile();
	return EXIT_SUCCESS;
}
