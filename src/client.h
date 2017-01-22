#ifndef CLIENT_H_
#define CLIENT_H

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

#include "network.h"
#include "Manette/controller.h"

typedef struct args_CLIENT {
	int port;
	char * adresse;
	PMutex * pmutex;
	args_CONTROLER * argControler;
} args_CLIENT;

void clean_args_CLIENT(args_CLIENT * arg);

char * dataControllerToMessage(int sizeFloat,DataController * dataController);
char* concat(const char *s1, const char *s2);

void *thread_UDP_CLIENT(void *args);
void *thread_TCP_CLIENT(void *args);

int startClientRemote(char * adresse);

#endif /* CLIENT_H_ */
