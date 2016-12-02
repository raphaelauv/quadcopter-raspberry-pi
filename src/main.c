
#include "serv.h"


int main() {

	boolMutex * boolConnectRemote = malloc(sizeof(boolMutex));
	boolConnectRemote->mutex = (pthread_mutex_t )PTHREAD_MUTEX_INITIALIZER;
	boolConnectRemote->condition = (pthread_cond_t ) PTHREAD_COND_INITIALIZER;
	boolConnectRemote->var = 0;

	char * adresse = malloc(sizeof(char) * 15);
	getIP(adresse);

	pthread_t threadServer;
	pthread_t threadMotor;
	pthread_t threadPid;

	args_SERVER * argServ = malloc(sizeof(args_SERVER));
	argServ->booleanMutex=boolConnectRemote;



	pthread_mutex_lock(&boolConnectRemote->mutex);

	if (pthread_create(&threadServer, NULL, thread_TCP_SERVER, argServ)) {
			perror("pthread_create");
			return EXIT_FAILURE;
	}

	pthread_cond_wait(&boolConnectRemote->condition, &boolConnectRemote->mutex);

	pthread_mutex_unlock(&boolConnectRemote->mutex);


	/*
	if (pthread_create(&threadMotor, NULL, ####, argServ)) {
			perror("pthread_create");
			return EXIT_FAILURE;
	}
	*/

	if (pthread_join(threadServer, NULL)){
		perror("pthread_join");
		return EXIT_FAILURE;
	}

	free(boolConnectRemote);
	free(argServ);
	return 0;
}
