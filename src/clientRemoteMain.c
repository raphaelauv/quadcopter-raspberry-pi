#include "client.h"
#include "Controller/controller.h"

volatile sig_atomic_t boolStopClient;
volatile sig_atomic_t boolStopController;

void handler_SIGINT_client(int i){
	boolStopClient=1;
	boolStopController=1;
	logString("THREAD MAIN : SIGINT catched -> process to stop");
}


int main(int argc, char *argv[]){

	init_mask(handler_SIGINT_client);

	if(argc<2){
		printf("ERROR : enter IP adresse of drone in argument\n");
		return EXIT_FAILURE;
	}
	char * adresse=argv[1];

	if (tokenAnalyse(argc, argv,FLAG_OPTIONS_CLIENT)) {
		printf("Error Args\n");
		return EXIT_FAILURE;
	}
	char array[SIZE_MAX_LOG];
	sprintf(array, "THREAD MAIN : adresse enter : %s",argv[1]);
	logString(array);

	args_CONTROLLER * argController;
	if(init_args_CONTROLLER(&argController,&boolStopController)){
		return EXIT_FAILURE;
	}

	args_CLIENT * argClient;	
	if(init_args_CLIENT(&argClient,adresse,argController,&boolStopClient)){
		return EXIT_FAILURE;
	}

	pthread_t threadClient;
	pthread_t threadController;

	if(isControl()){

		logString("THREAD MAIN : TEST CONTROLER");

		pthread_mutex_lock(&argController->pmutexControllerPlug->mutex);

		if (pthread_create(&threadController, NULL, thread_CONTROLLER,argController)) {
			logString("THREAD MAIN : ERROR pthread_create thread_CONTROLER");
			return EXIT_FAILURE;
		}

		//wait for CONTROLER
		pthread_cond_wait(&argController->pmutexControllerPlug->condition, &argController->pmutexControllerPlug->mutex);

		pthread_mutex_unlock(&argController->pmutexControllerPlug->mutex);
	}

	if(!is_Controller_Stop(argController)){

			//CONTROLER IS ON , we can start the client socket thread
			if (pthread_create(&threadClient, NULL, thread_UDP_CLIENT, argClient)) {
				logString("THREAD MAIN : ERROR pthread_create thread_UDP_CLIENT");
				return EXIT_FAILURE;
			}

			if (pthread_join(threadClient, NULL)) {
				logString("THREAD MAIN : ERROR pthread_join thread_UDP_CLIENT");
				set_Client_Stop(argClient);
				return EXIT_FAILURE;
			}

			set_Controller_Stop(argController);

			if (isControl()) {
				if (pthread_join(threadController, NULL)) {
					logString("THREAD MAIN : ERROR pthread_join CONTROLER");
					return EXIT_FAILURE;
				}
			}
	}

	clean_args_CLIENT(argClient);
	clean_args_CONTROLLER(argController);
	logString("THREAD MAIN : END");

	closeLogFile();
	return EXIT_SUCCESS;
}
