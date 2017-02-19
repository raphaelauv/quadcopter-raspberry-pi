#include "client.h"
#include "Manette/controller.h"

int main (int argc, char *argv[]){
	if(argc<2){
		perror("put IP adresse of drone in argument");
		return EXIT_FAILURE;
	}
	char * adresse=argv[1];
	char verbose=0;
	if (setVerboseOrLog(&verbose, argc, argv[2], 2)) {
		return EXIT_FAILURE;
	}

	char array[400];
	sprintf(array, "THREAD MAIN : adresse enter : %s",argv[1]);
	logString(array);

	args_CONTROLER * argControler;
	if(init_args_CONTROLER(&argControler)){
		return EXIT_FAILURE;
	}

	args_CLIENT * argClient;
	if(init_args_CLIENT(&argClient,adresse,argControler)){
		return EXIT_FAILURE;
	}

	pthread_t threadClient;
	pthread_t threadControler;

	logString("THREAD MAIN : TEST CONTROLER");

	pthread_mutex_lock(&argControler->pmutexControllerPlug->mutex);

	if (pthread_create(&threadControler, NULL, thread_CONTROLER,argControler)) {
		logString("THREAD MAIN : ERROR pthread_create thread_CONTROLER");
		return EXIT_FAILURE;
	}

	//wait for CONTROLER
	pthread_cond_wait(&argControler->pmutexControllerPlug->condition, &argControler->pmutexControllerPlug->mutex);

	pthread_mutex_unlock(&argControler->pmutexControllerPlug->mutex);

	//CONTROLER IS ON , we can start the client socket thread
	if (pthread_create(&threadClient, NULL, thread_UDP_CLIENT, argClient)) {
		logString("THREAD MAIN : ERROR pthread_create thread_UDP_CLIENT");
		return EXIT_FAILURE;
	}

	if (pthread_join(threadClient, NULL)) {
		logString("THREAD MAIN : ERROR pthread_join thread_UDP_CLIENT");
		return EXIT_FAILURE;
	}

	argControler->endController=1;

	if (pthread_join(threadControler, NULL)) {
		logString("THREAD MAIN : ERROR pthread_join CONTROLER");
		return EXIT_FAILURE;
	}

	clean_args_CLIENT(argClient);
	clean_args_CONTROLER(argControler);
	logString("THREAD MAIN : END");

	closeLogFile();
	return EXIT_SUCCESS;
}
