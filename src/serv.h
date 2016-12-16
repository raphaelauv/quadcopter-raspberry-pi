#ifndef SERV_H_
#define SERV_H_

#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <pthread.h>
#include <sys/select.h>
#include <fcntl.h>


#include "concurrent.h"

typedef struct args_SERVER {
	boolMutex * booleanMutex;

} args_SERVER;

void clean_args_SERVER(args_SERVER * arg);

void getIP(char * adresse);
void *thread_TCP_SERVER(void *args);

#endif /* SERV_H_ */
