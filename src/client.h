#ifndef CLIENT_H_
#define CLIENT_H

#include "concurrent.h"
#include "network.h"
#include "Manette/controller.h"

typedef struct args_CLIENT {
	char verbose;
	int sock;
	char * adresse;
	PMutex * pmutex;
	struct sockaddr_in * adr_client;
	args_CONTROLER * argControler;
} args_CLIENT;

int init_args_CLIENT(args_CLIENT ** argClient,char * adresse,args_CONTROLER * argControler,char verbose);
void clean_args_CLIENT(args_CLIENT * arg);

void dataControllerToMessage(int sizeFloat,char * output,DataController * dataController);
void concat(const char *typeMsg,const char *s1, const char *s2, char * messageWithInfo);

void *thread_UDP_CLIENT(void *args);
void *thread_TCP_CLIENT(void *args);
void *thread_XBOX_CONTROLER(void *args) ;

#endif /* CLIENT_H_ */
