#ifndef CLIENT_H_
#define CLIENT_H

#include "concurrent.h"
#include "network.h"
#include "Controller/controller.h"

typedef struct args_CLIENT {
	char verbose;
	int sock;
	char * adresse;
	//volatile int * boolStopClient;
	volatile sig_atomic_t * boolStopClient;
	PMutex * pmutex;
	struct sockaddr_in * adr_client;
	args_CONTROLLER * argController;
} args_CLIENT;

int init_args_CLIENT(args_CLIENT ** argClient,char * adresse,args_CONTROLLER * argController,volatile sig_atomic_t * boolStopClient);
void clean_args_CLIENT(args_CLIENT * arg);

void dataControllerToMessage(int sizeFloat,char * output,DataController * dataController);
void concat(const char *typeMsg,const char *s1, const char *s2, char * messageWithInfo);

void *thread_UDP_CLIENT(void *args);

#endif /* CLIENT_H_ */
