#ifndef CLIENT_H_
#define CLIENT_H

#include <sys/time.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "concurrent.h"


typedef struct dataController{
	char moteur_active;
	float moteur0;
	float moteur1;
	float moteur2;
	float moteur3;
}dataController;

typedef struct args_CONTROLER {
	int new;
	struct dataController * manette;
	boolMutex * mutexReadDataController;
	boolMutex * mutexControlerPlug;
} args_CONTROLER;


typedef struct args_CLIENT {
	int port;
	char * adresse;
	boolMutex * booleanMutex;
	args_CONTROLER * argControler;
} args_CLIENT;





#endif /* CLIENT_H_ */
