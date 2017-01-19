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

#include "Manette/controller.h"

typedef struct args_CLIENT {
	int port;
	char * adresse;
	PMutex * pmutex;
	args_CONTROLER * argControler;
} args_CLIENT;



#endif /* CLIENT_H_ */
