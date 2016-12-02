#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "serv.h"

int main() {

	char * adresse = malloc(sizeof(char) * 15);
	getIP(adresse);

	pthread_t threadServer;
	pthread_t threadMotor;
	pthread_t threadPid;

	if (pthread_create(&threadServer, NULL, thread_TCP_SERVER, NULL)) {
		perror("pthread_create");
		return EXIT_FAILURE;
	}

	if (pthread_join(threadServer, NULL)) {
		perror("pthread_join");
		return EXIT_FAILURE;
	}



	return 0;
}
