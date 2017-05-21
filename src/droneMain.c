#include "serv.h"
#include "motors.h"
//#include "old/MonoThreadMotor.h"
#include "PID.hpp"
#include "Calibration/calibrate.h"

volatile sig_atomic_t signalMotorStop;
volatile sig_atomic_t signalServStop;


void handler_SIGINT_Drone(int i){
	signalServStop=1;
	signalMotorStop=1;
	logString("THREAD MAIN : SIGINT catched -> process to stop");
}

int main (int argc, char *argv[]){

	int exitValue=0;
	pthread_t threadServer;
	pthread_t threadPID;
	void *threadPID_stack_buf = NULL;

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

	args_SERVER * argServ = NULL;
	MotorsAll * motorsAll = NULL;
	args_PID * argPID = NULL;

	if(init_args_SERVER(&argServ,&signalServStop)){
		return EXIT_FAILURE;
	}

	if (init_MotorsAll(&motorsAll,&signalMotorStop)) {
		exitValue=1;
		goto cleanAndExit;
	}
	
	if (init_args_PID(&argPID)) {
		exitValue=1;
		goto cleanAndExit;
	}

	argPID->dataController=argServ->dataController;
	argPID->pidInfo=argServ->pidInfo;
	argPID->motorsAll=motorsAll;



	if(isControl()){
		if(pthread_mutex_lock(&argServ->pmutexRemoteConnect->mutex)){
			exitValue=1;
			goto cleanAndExit;
		}

		if (pthread_create(&threadServer, NULL, thread_UDP_SERVER, argServ)) {
			logString("THREAD MAIN : ERROR pthread_create SERVER\n");
			set_Motor_Stop(motorsAll);
			exitValue=1;
			goto cleanAndExit;
		}

		pthread_cond_wait(&argServ->pmutexRemoteConnect->condition, &argServ->pmutexRemoteConnect->mutex);

		if(pthread_mutex_unlock(&argServ->pmutexRemoteConnect->mutex)){
			exitValue=1;
			goto cleanAndExit;
		}

		if (is_Serv_Stop(argServ)) {
			exitValue=1;
			goto cleanAndExit;
		}
	}

	if(start_thread_PID(&threadPID,threadPID_stack_buf,argPID)){
		logString("THREAD MAIN : ERROR pthread_create PID\n");
		set_Motor_Stop(motorsAll);
		set_Serv_Stop(argServ);
		exitValue=1;
		goto cleanAndExit;
	}

	/**
	 * AFTER THE CALIBRATION THE PROGRAM FINISH
	 */
	if (isCalibration()) {

		if(isTestpower()){
			test_Power(motorsAll);
		}else{
			calibrate_ESC(motorsAll, isVerbose());
		}
		set_Motor_Stop(motorsAll);
		set_Serv_Stop(argServ);
	}


	if (pthread_join(threadPID, NULL)) {
		logString("THREAD MAIN : ERROR pthread_join PID");
		set_Motor_Stop(motorsAll);
		set_Serv_Stop(argServ);
		exitValue=1;
		goto cleanAndExit;
	}


	if (isControl()) {
		if (pthread_join(threadServer, NULL)) {
			logString("THREAD MAIN : ERROR pthread_join SERVER");
			set_Motor_Stop(motorsAll);
			set_Serv_Stop(argServ);
			exitValue=1;
			goto cleanAndExit;
		}
	}

cleanAndExit:

	if(threadPID_stack_buf!=NULL){
		//munmap(threadPID_stack_buf, PTHREAD_STACK_MIN);
	}

	clean_args_SERVER(argServ);
	clean_args_PID(argPID);
	clean_MotorsAll(motorsAll);

	logString("THREAD MAIN : END");
	closeLogFile();
	return exitValue;

}
