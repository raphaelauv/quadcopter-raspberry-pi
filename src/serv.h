#ifndef SERV_H_
#define SERV_H_

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

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

#include "network.h"
#include "concurrent.h"

typedef struct args_SERVER {
	char verbose;
	int sock;
	PMutex * pmutexRemoteConnect;
	DataController * dataController;

} args_SERVER;

int initArgServ(args_SERVER ** argServ,char verbose);

void clean_args_SERVER(args_SERVER * arg);

void *thread_TCP_SERVER(void *args);
void *thread_UDP_SERVER(void *args);

/*
#ifdef __cplusplus
}
#endif
*/
#endif /* SERV_H_ */
