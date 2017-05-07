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

	int exitValue=0;
	pthread_t threadController;
	pthread_t threadClient;

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
		exitValue=1;
		goto cleanAndExit;
	}



	if(isControl()){

		logString("THREAD MAIN : TEST CONTROLER");

		pthread_mutex_lock(&argController->pmutexControllerPlug->mutex);
		//TODO verif

		if (pthread_create(&threadController, NULL, thread_CONTROLLER,argController)) {
			logString("THREAD MAIN : ERROR pthread_create thread_CONTROLER");
			pthread_mutex_unlock(&argController->pmutexControllerPlug->mutex);
			exitValue=1;
			goto cleanAndExit;
		}

		//wait for CONTROLER
		pthread_cond_wait(&argController->pmutexControllerPlug->condition, &argController->pmutexControllerPlug->mutex);
		//TODO

		pthread_mutex_unlock(&argController->pmutexControllerPlug->mutex);
		//TODO
	}

	if(!is_Controller_Stop(argController)){

			//CONTROLER IS ON , we can start the client socket thread
			if (pthread_create(&threadClient, NULL, thread_UDP_CLIENT, argClient)) {
				logString("THREAD MAIN : ERROR pthread_create thread_UDP_CLIENT");
				exitValue=1;
				goto cleanAndExit;
			}

			if (pthread_join(threadClient, NULL)) {
				logString("THREAD MAIN : ERROR pthread_join thread_UDP_CLIENT");
				set_Client_Stop(argClient);
				exitValue=1;
				goto cleanAndExit;
			}

			set_Controller_Stop(argController);

			if (isControl()) {
				if (pthread_join(threadController, NULL)) {
					logString("THREAD MAIN : ERROR pthread_join CONTROLER");
					exitValue=1;
					goto cleanAndExit;
				}
			}
	}



cleanAndExit:
	clean_args_CONTROLLER(argController);
	clean_args_CLIENT(argClient);
	logString("THREAD MAIN : END");
	closeLogFile();
	return exitValue;
}
