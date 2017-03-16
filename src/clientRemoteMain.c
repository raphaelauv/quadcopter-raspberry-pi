#include "client.h"
#include "Controller/controller.h"
#include <sys/signal.h>

#define ERROR(a,str) if (a<0) {perror(str); exit(1);}

volatile int * boolStopClient=NULL;

void stopNetwork(){
	logString("THREAD MAIN : SIGINT catched -> process to stop");
	*boolStopClient=1;
	sleep(3);
	exit(EXIT_FAILURE);
}

void handler(int i){
  boolStopClient==NULL ? exit(EXIT_FAILURE) : stopNetwork();
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

	if(argc<2){
		perror("put IP adresse of drone in argument");
		return EXIT_FAILURE;
	}
	char * adresse=argv[1];

	if (tokenAnalyse(argc, argv)) {
		return EXIT_FAILURE;
	}

	char array[SIZE_MAX_LOG];
	sprintf(array, "THREAD MAIN : adresse enter : %s",argv[1]);
	logString(array);

	args_CONTROLLER * argController;
	if(init_args_CONTROLLER(&argController)){
		return EXIT_FAILURE;
	}

	args_CLIENT * argClient;	
	if(init_args_CLIENT(&argClient,adresse,argController)){
		return EXIT_FAILURE;
	}

	boolStopClient = argClient->boolStopClient;

	pthread_t threadClient;
	pthread_t threadController;

	logString("THREAD MAIN : TEST CONTROLER");

	pthread_mutex_lock(&argController->pmutexControllerPlug->mutex);

	if (pthread_create(&threadController, NULL, thread_CONTROLLER,argController)) {
		logString("THREAD MAIN : ERROR pthread_create thread_CONTROLER");
		return EXIT_FAILURE;
	}

	//wait for CONTROLER
	pthread_cond_wait(&argController->pmutexControllerPlug->condition, &argController->pmutexControllerPlug->mutex);

	pthread_mutex_unlock(&argController->pmutexControllerPlug->mutex);

	//CONTROLER IS ON , we can start the client socket thread
	if (pthread_create(&threadClient, NULL, thread_UDP_CLIENT, argClient)) {
		logString("THREAD MAIN : ERROR pthread_create thread_UDP_CLIENT");
		return EXIT_FAILURE;
	}

	if (pthread_join(threadClient, NULL)) {
		logString("THREAD MAIN : ERROR pthread_join thread_UDP_CLIENT");
		stopNetwork();
		return EXIT_FAILURE;
	}

	argController->endController=1;

	if (pthread_join(threadController, NULL)) {
		logString("THREAD MAIN : ERROR pthread_join CONTROLER");
		return EXIT_FAILURE;
	}

	clean_args_CLIENT(argClient);
	clean_args_CONTROLLER(argController);
	logString("THREAD MAIN : END");

	closeLogFile();
	return EXIT_SUCCESS;
}
