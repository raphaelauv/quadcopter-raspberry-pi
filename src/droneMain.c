#include "serv.h"
#include "motors.h"
#include "PID.hpp"
#include "Calibration/calibrate.h"

MotorsAll2 * GlobalMotorAll=NULL;
volatile int * boolStopServ=NULL;

void stopMotor(){
	if(GlobalMotorAll!=NULL){
		setMotorStop(GlobalMotorAll);
	}
}

void stopNetworkServ(){
	if(boolStopServ!=NULL){
		*boolStopServ=1;
	}
}

void drone_stopAll(){
	stopMotor();
	stopNetworkServ();
}


void handler_SIGINT_Drone(int i){
	logString("THREAD MAIN : SIGINT catched -> process to stop");
	drone_stopAll();
}

int main (int argc, char *argv[]){
	
	init_mask(handler_SIGINT_Drone);
	
	if(tokenAnalyse(argc,argv,FLAG_OPTIONS_DRONE)){
		return EXIT_FAILURE;
	}

	char myIP[64];
	if(getIP(myIP)){
		return EXIT_FAILURE;	
	}
	if (isIpSound()) {	
		readIpAdresse(myIP, 64);
	}

	args_SERVER * argServ;
	if(init_args_SERVER(&argServ)){
		return EXIT_FAILURE;
	}

	boolStopServ = argServ->boolStopServ;

	MotorsAll2 * motorsAll2;
	if (init_MotorsAll2(&motorsAll2)) {
		return EXIT_FAILURE;
	}

	GlobalMotorAll=motorsAll2;


	args_PID * argPID;
	if (init_args_PID(&argPID,argServ->dataController,motorsAll2)) {
		return EXIT_FAILURE;
	}

	pthread_t threadServer;
	pthread_t threadPID;
	pthread_t threadMotor2;

	if(!isNoControl()){
		pthread_mutex_lock(&argServ->pmutexRemoteConnect->mutex);

		if (pthread_create(&threadServer, NULL, thread_UDP_SERVER, argServ)) {
			logString("THREAD MAIN : pthread_create SERVER\n");
			return EXIT_FAILURE;
		}

		pthread_cond_wait(&argServ->pmutexRemoteConnect->condition, &argServ->pmutexRemoteConnect->mutex);

		pthread_mutex_unlock(&argServ->pmutexRemoteConnect->mutex);
	}

	if(init_thread_PID(&threadPID,argPID)){
		stopNetworkServ();
		return EXIT_FAILURE;
	}

	if(init_thread_startMotorAll2(&threadMotor2,motorsAll2)){
		drone_stopAll();
		return EXIT_FAILURE;
	}


	/**
	 * AFTER THE CALIBRATION THE PROGRAM FINISH
	 */
	if(isCalibration()){
		calibrate_ESC(motorsAll2,isVerbose());
		drone_stopAll();
		sleep(1);//TODO
		return EXIT_SUCCESS;
	}


	int * returnValue;

	int re=0;

	if (pthread_join(threadPID, (void**) &returnValue)){
		logString("THREAD MAIN : ERROR pthread_join PID");
		drone_stopAll();
		return EXIT_FAILURE;
	}

	if (pthread_join(threadMotor2, (void**) &returnValue)) {
		logString("THREAD MAIN : ERROR pthread_join MOTOR");
		drone_stopAll();
		return EXIT_FAILURE;
	}

	if(!isNoControl()){
		if ((re=pthread_join(threadServer,NULL))>0) {
				logString("THREAD MAIN : ERROR pthread_join SERVER");
				return EXIT_FAILURE;
		}
	}


	clean_args_SERVER(argServ);
	clean_args_PID(argPID);

	logString("THREAD MAIN : END");
	closeLogFile();
	return EXIT_SUCCESS;
}
