#ifndef NETWORK_H_
#define NETWORK_H_
/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#include <arpa/inet.h>
#include <asm-generic/socket.h>

#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "concurrent.h"

#define SIZE_SOCKET_MESSAGE 100

#define SIZE_MSG_HEADER_REMOTE 7

#define SIZE_MSG_HEADER_DATA 5

#define SIZE_MSG_HEADER_PAUSE 5

#define SIZE_MSG_HEADER_STOP 4

#define STR_REMOTE "REMOTE "

#define STR_DATA "DATA "

#define STR_PAUSE "PAUSE"

#define STR_STOP "STOP"

#define UDP_PORT_DRONE 8898

#define UDP_PORT_REMOTE 8891

#define UDP_TIME_SEC_TIMER 1

#define UDP_TIME_USEC_TIMER 500000

char get_IP_Port(char *message,struct sockaddr_in * sa);

char isMessageRemote(char * message);

char isMessageData(char * message);

char isMessageStop(char * message);

char isMessagePause(char * message);

int bindUDPSock(int * sock, struct sockaddr_in * adr_svr);

int receveNetwork(int sock, struct sockaddr_in *adr_svr, char * message);

int sendNetwork(int sock,struct sockaddr_in *adr_svr,char * message);

void getIP(char*  myIP);

#define SOUND_PLAYER "mpg123 -q "
#ifdef __arm__
#define SOUND_FOLDER "/home/pi/drone/Lib/sound/"
#else
#define SOUND_FOLDER " ~/drone/Lib/sound/"
#endif

#define SOUND_COMMAND SOUND_PLAYER SOUND_FOLDER

#define SOUND_PAUSE_TIME 400000
void readIpAdresse(char * ipAdresse,int size);

/*
#ifdef __cplusplus
}
#endif
*/
#endif /* NETWORK_H_ */
