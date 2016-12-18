#include "serv.h"
#include "motors.h"
#include "controldeVol.h"

int main() {


	boolMutex * boolConnectRemote = malloc(sizeof(boolMutex));
	init_boolMutex(boolConnectRemote);

	boolMutex * mutexReadmotors = malloc(sizeof(boolMutex));
	init_boolMutex(mutexReadmotors);

	boolMutex * mutexDataControler = malloc(sizeof(boolMutex));
	init_boolMutex(mutexDataControler);


	char * adresse = malloc(sizeof(char) * 15);
	getIP(adresse);

	pthread_t threadServer;
	pthread_t threadControlerVOL;

	dataController * dataController = malloc(sizeof(dataController));

	args_SERVER * argServ = malloc(sizeof(args_SERVER));
	argServ->boolConnectRemote = boolConnectRemote;
	argServ->mutexDataControler=mutexDataControler;
	argServ->dataController = dataController;


	motorsAll * motorsAll = malloc(sizeof(motorsAll));
	motorsAll->bool_arret_moteur = 0;
	motorsAll->mutexReadmotors = mutexReadmotors;


	args_CONTROLDEVOL * argCONTROLVOL = malloc(sizeof(args_CONTROLDEVOL));
	argCONTROLVOL->mutexDataControler=mutexDataControler;
	argCONTROLVOL->dataController=dataController;
	argCONTROLVOL->motorsAll=motorsAll;


	pthread_mutex_lock(&boolConnectRemote->mutex);

	if (pthread_create(&threadServer, NULL, thread_TCP_SERVER, argServ)) {
		perror("pthread_create");
		return EXIT_FAILURE;
	}

	pthread_cond_wait(&boolConnectRemote->condition, &boolConnectRemote->mutex);

	pthread_mutex_unlock(&boolConnectRemote->mutex);

	if (pthread_create(&threadControlerVOL, NULL, startCONTROLVOL, argCONTROLVOL)) {
				perror("pthread_create");
				return EXIT_FAILURE;
	}

	init_motors(motorsAll);//start the 4 threads et ne rends pas la main


	if (pthread_join(threadServer, NULL)){
		perror("pthread_join");
		return EXIT_FAILURE;
	}
	if (pthread_join(threadControlerVOL, NULL)){
			perror("pthread_join");
			return EXIT_FAILURE;
	}

	free(boolConnectRemote);
	free(argServ);
	return 0;
}
