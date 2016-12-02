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

void getIP(char * adresse);
void *thread_TCP_SERVER(void *arg);


#endif /* SERV_H_ */
