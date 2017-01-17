#include "serv.h"
#include "motors.h"
#include "controldeVol.h"

int main() {

	PMutex * PmutexRemoteConnect = malloc(sizeof(PMutex));
	init_PMutex(PmutexRemoteConnect);

	PMutex * PmutexDataControler = malloc(sizeof(PMutex));
	init_PMutex(PmutexDataControler);

	getIP();

	DataController * dataControl = malloc(sizeof(DataController));
	dataControl->pmutex=PmutexDataControler;

	args_SERVER * argServ = malloc(sizeof(args_SERVER));
	argServ->pmutexRemoteConnect = PmutexRemoteConnect;
	argServ->dataController = dataControl;

	MotorsAll * motorsAll = malloc(sizeof(MotorsAll));
	motorsAll->bool_arret_moteur = malloc(sizeof(int));
	*(motorsAll->bool_arret_moteur)= 0;

	init_Value_motors(motorsAll);


	args_CONTROLDEVOL * argCONTROLVOL = malloc(sizeof(args_CONTROLDEVOL));
	argCONTROLVOL->dataController=dataControl;
	argCONTROLVOL->motorsAll=motorsAll;

	pthread_t threadServer;
	pthread_t threadControlerVOL;


	pthread_mutex_lock(&PmutexRemoteConnect->mutex);

	if (pthread_create(&threadServer, NULL, thread_TCP_SERVER, argServ)) {
		perror("pthread_create TCP");
		return EXIT_FAILURE;
	}

	pthread_cond_wait(&PmutexRemoteConnect->condition, &PmutexRemoteConnect->mutex);

	pthread_mutex_unlock(&PmutexRemoteConnect->mutex);


	if (pthread_create(&threadControlerVOL, NULL, startCONTROLVOL, argCONTROLVOL)) {
		perror("pthread_create PID");
		return EXIT_FAILURE;
	}


	init_threads_motors(motorsAll);//start the 4 threads et ne rends pas la main


	if (pthread_join(threadServer, NULL)){
		perror("pthread_join");
		return EXIT_FAILURE;
	}


	if (pthread_join(threadControlerVOL, NULL)){
			perror("pthread_join");
			return EXIT_FAILURE;
	}


	clean_args_SERVER(argServ);
	clean_args_CONTROLDEVOL(argCONTROLVOL);
	return 0;
}
