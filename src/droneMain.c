#include "serv.h"
#include "motors.h"
#include "PID.hpp"
//#include "signal.h"
#include <sys/signal.h>

#define ERROR(a,str) if (a<0) {perror(str); exit(1);}

volatile int * boolStopServ=NULL;
volatile int * boolStopMotor=NULL;

void stopMotor(){
	if(boolStopMotor!=NULL){
		*boolStopMotor=1;
	}
}

void stopNetwork(){
	if(boolStopServ!=NULL){
		*boolStopServ=1;
	}
}

void stopAll(){
	logString("THREAD MAIN : SIGINT catched -> process to stop");
	stopNetwork();
	stopMotor();
	sleep(3);
	exit(EXIT_FAILURE);
}

void handler(int i){
  boolStopServ==NULL ? exit(EXIT_FAILURE) : stopAll();
}

void init_mask(){
  int rc;
  
  struct sigaction sa;
  memset(&sa,0,sizeof(sa));
  sa.sa_flags = 0;

  sa.sa_handler = handler;

  sigset_t set;
  rc = sigemptyset(&set);
  ERROR(rc,"sigemptyset\n");
  rc = sigaddset(&set,SIGQUIT);
  ERROR(rc, "sigaddset\n");

  sa.sa_mask = set;
  
  rc = sigaction(SIGINT, &sa, NULL);
  ERROR(rc,"sigaction\n");

}

int main (int argc, char *argv[]){
	
	init_mask();
  
	char noControl=0;
	if(setVerboseOrLog(argc,argv[1],1)){
		return EXIT_FAILURE;
	}
	setNoControl(&noControl,argc,argv[2],2);

	args_SERVER * argServ;
	if(init_args_SERVER(&argServ)){
		return EXIT_FAILURE;
	}

	boolStopServ = argServ->boolStopServ;

	
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

	boolStopMotor=motorsAll2->bool_arret_moteur;

	args_PID * argPID;
	if (init_args_PID(&argPID,argServ->dataController,motorsAll2)) {
		return EXIT_FAILURE;
	}

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
		stopNetwork();
		return EXIT_FAILURE;
	}


	if(init_thread_startMotorAll2(&threadMotor2,motorsAll2)){
		stopNetwork();
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
		stopNetwork();
		return EXIT_FAILURE;
	}

	if (pthread_join(threadMotor2, (void**) &returnValue)) {
		logString("THREAD MAIN : ERROR pthread_join MOTOR");
		stopNetwork();
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
