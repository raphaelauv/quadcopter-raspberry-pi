#include "serv.h"
#include "motors.h"
#include "controldeVol.hpp"
#include "capteur.hpp"


int main (int argc, char *argv[]){

	char verbose;
	setVerbose(&verbose,argc,argv[1],1);

	args_SERVER * argServ;
	if(initArgServ(&argServ)){
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
		perror("NEW FAIL : RTIMU ->imu\n");
		return EXIT_FAILURE;
	}else{
		if(verbose){printf("THREAD MAIN : CAPTEUR INIT SUCCES\n");}
		argCONTROLVOL->imu=imu;
	}
	#endif

	pthread_t threadServer;
	pthread_t threadPID;


	pthread_mutex_lock(&argServ->pmutexRemoteConnect->mutex);

	if (pthread_create(&threadServer, NULL, thread_UDP_SERVER, argServ)) {
		logString("THREAD MAIN : pthread_create SERVER\n");
		return EXIT_FAILURE;
	}

	pthread_cond_wait(&argServ->pmutexRemoteConnect->condition, &argServ->pmutexRemoteConnect->mutex);

	pthread_mutex_unlock(&argServ->pmutexRemoteConnect->mutex);

	if(init_thread_PID(&threadPID,argCONTROLVOL)){
		//TODO demander fermeture reseaux
		return EXIT_FAILURE;
	}

	//start the 4 threads et ne rends pas la main si succes
	if(init_threads_motors(motorsAll,verbose)){
		//TODO demander fermeture reseaux
		return EXIT_FAILURE;
	}

	int * returnValue;

	if (pthread_join(threadServer,(void**) &returnValue)){
		logString("THREAD MAIN : pthread_join SERVER\n");
		return EXIT_FAILURE;
	}
	if (pthread_join(threadPID, (void**) &returnValue)){
		logString("THREAD MAIN : pthread_join PID\n");
		return EXIT_FAILURE;
	}

	clean_args_SERVER(argServ);
	clean_args_CONTROLDEVOL(argCONTROLVOL);

	logString("THREAD MAIN : END\n");
	return EXIT_SUCCESS;
}
