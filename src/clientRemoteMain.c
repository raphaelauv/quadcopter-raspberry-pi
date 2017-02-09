#include "client.h"

int main (int argc, char *argv[]){
	if(argc<2){
		perror("Indiquer l'adresse IP du drone en argument");
		return EXIT_FAILURE;
	}
	char verbose=0;
	setVerbose(&verbose,argc,argv[2],2);

	printf("adresse choisit : %s\n",argv[1]);

	char * adresse=argv[1];

	PMutex * pmutexControllerPlug =(PMutex *) malloc(sizeof(PMutex));
	if (pmutexControllerPlug == NULL) {
		perror("MALLOC FAIL : pmutexControllerPlug\n");
		return EXIT_FAILURE;
	}
	init_PMutex(pmutexControllerPlug);


	PMutex * pmutexRead =(PMutex *) malloc(sizeof(PMutex));
	if (pmutexRead == NULL) {
		perror("MALLOC FAIL : pmutexRead\n");
		return EXIT_FAILURE;
	}
	init_PMutex(pmutexRead);

	args_CONTROLER * argControler =(args_CONTROLER *) malloc(sizeof(args_CONTROLER));
	if (argControler == NULL) {
		perror("MALLOC FAIL : argControler\n");
		return EXIT_FAILURE;
	}
	argControler->newThing=0;
	argControler->endController=0;
	argControler->manette=(DataController *) malloc(sizeof( DataController));
	if (argControler->manette == NULL) {
		perror("MALLOC FAIL : argControler->manette\n");
		return EXIT_FAILURE;
	}
	argControler->pmutexReadDataController=pmutexRead;
	argControler->pmutexControlerPlug=pmutexControllerPlug;
	argControler->verbose=verbose;

	args_CLIENT * argClient =(args_CLIENT *) malloc(sizeof(args_CLIENT));
	if (argClient == NULL) {
		perror("MALLOC FAIL : argClient\n");
		return EXIT_FAILURE;
	}
	argClient->port=UDP_PORT_DRONE;
	argClient->adresse=adresse;
	argClient->verbose=verbose;

	argClient->argControler=argControler;

	pthread_t threadClient;
	pthread_t threadControler;

	if(verbose){printf("TEST MANETTE\n");}

	pthread_mutex_lock(&pmutexControllerPlug->mutex);

	if (pthread_create(&threadControler, NULL, thread_XBOX_CONTROLER,argControler)) {
		perror("pthread_create");
		return EXIT_FAILURE;
	}

	//wait for XBOX CONTROLER
	pthread_cond_wait(&pmutexControllerPlug->condition, &pmutexControllerPlug->mutex);

	pthread_mutex_unlock(&pmutexControllerPlug->mutex);

	if(verbose){printf("MANETTE ok \n");}

	//XBOX CONTROLER IS ON , we can start the client socket thread
	if (pthread_create(&threadClient, NULL, thread_UDP_CLIENT, argClient)) {
		perror("pthread_create");
		return EXIT_FAILURE;
	}

	if (pthread_join(threadClient, NULL)) {
		perror("pthread_join SERV");
		return EXIT_FAILURE;
	}

	argControler->endController=1;

	if (pthread_join(threadControler, NULL)) {
		perror("pthread_join CONTROLER");
		return EXIT_FAILURE;
	}


	clean_args_CLIENT(argClient);
	clean_args_CONTROLER(argControler);
	if(verbose){printf("THREAD MAIN : END\n");}

	return EXIT_SUCCESS;
}
