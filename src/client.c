#include "client.h"

void clean_args_CLIENT(args_CLIENT * arg) {
	if (arg != NULL) {
		clean_PMutex(arg->pmutex);
		clean_args_CONTROLER(arg->argControler);
		free(arg->adresse);
	}
	free(arg);
	arg = NULL;
}


void dataControllerToMessage(int sizeFloat,char * output,DataController * dataController){


	int tmp=sizeFloat;

	snprintf(output, sizeFloat, "%f", dataController->axe_Rotation);

	output[tmp-2]=' ';

	snprintf(output+tmp-1, sizeFloat, "%f", dataController->axe_UpDown);

	tmp+=sizeFloat-1;

	output[tmp-2]=' ';

	snprintf(output+tmp-1, sizeFloat, "%f", dataController->axe_LeftRight);

	tmp+=sizeFloat-1;

	output[tmp-2]=' ';

	snprintf(output+tmp-1, sizeFloat, "%f", dataController->axe_FrontBack);

	tmp+=sizeFloat-1;

	output[tmp-2]=' ';

	snprintf(output+tmp-1, sizeFloat, "%d", dataController->flag);

	//printf("LE MESSAGE CREER EST : %s\n", output);

	//return output;
}


void concat(const char *s1, const char *s2, char * messageWithInfo){

	/*
	char *result;
    if((result=(char *)malloc(strlen(s1)+strlen(s2)+2))==NULL){
    	return NULL;
    }
	 */
    char space[2];
    space[0]=' ';
    space[1]=0;
    strcpy(messageWithInfo, s1);
    strcat(messageWithInfo,(const char *) &space);
    strcat(messageWithInfo, s2);

    //return result;
}

void testCloseDrone(int sock,char * messageFlagStop){


}


void *thread_UDP_CLIENT(void *args) {

	args_CLIENT * argClient = (args_CLIENT *) args;
	char verbose=argClient->verbose;
	if(verbose){printf("CLIENT UDP\n");}

	int sock;
	struct sockaddr_in adr_svr;
	memset(&adr_svr, 0, sizeof(adr_svr));
	adr_svr.sin_port	=htons(argClient->port);
	adr_svr.sin_family	=AF_INET;
	inet_aton(argClient->adresse, &adr_svr.sin_addr);


	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("Socket error\n");
		//TODO
	}

	//adr_my is for reception from drone
	struct sockaddr_in adr_my;
	memset(&adr_my, 0, sizeof(adr_my));
	adr_svr.sin_family 		= AF_INET;
	adr_svr.sin_addr.s_addr = htonl(INADDR_ANY);
	adr_svr.sin_port 		= htons(8888);

	if(bindUDPSock(&sock,&adr_my)==0){
		//TODO ERROR
	}

	if(fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK) < 0) {
		perror("Socket error\n");
		//TODO
	}

	char str[15];
	sprintf(str, "%d", argClient->port);

	char myIP[64];

	//int sizeMessageInfo=15+64;
	char messageWithInfo[SIZE_SOCKET_MESSAGE];


	getIP(myIP);
	if(myIP!=NULL){
		concat(myIP,str,messageWithInfo);
		messageWithInfo[SIZE_SOCKET_MESSAGE-1]='\0';

		if(sendNetwork(sock,&adr_svr,messageWithInfo)==0){
			//TODO ERROR;
		}

		//sendto(sock, messageWithInfo,SIZE_SOCKET_MESSAGE, 0, (struct sockaddr *) &adr_svr,sizeof(struct sockaddr_in));
		if(verbose){printf("THREAD CLIENT SENDED : %s\n", messageWithInfo);}

	}else{
		//TODO
	}

	int continu = 1;
	const int sizeFLOAT=10;
	//int sizeMessage=5*sizeFLOAT;
	char message[SIZE_SOCKET_MESSAGE];
	int resultWait;

	int flag;
	while (continu) {


		struct timespec timeToWait;
		struct timeval now;

		int timeInMs=1;

		gettimeofday(&now, NULL);

		timeToWait.tv_sec = now.tv_sec + 1;
		timeToWait.tv_nsec = (now.tv_usec + 1000UL * timeInMs) * 1000UL;


		pthread_mutex_lock(&argClient->argControler->pmutexReadDataController->mutex);

		if(argClient->argControler->newThing==0){

			resultWait=pthread_cond_timedwait(&argClient->argControler->pmutexReadDataController->condition,
					&argClient->argControler->pmutexReadDataController->mutex,&timeToWait);

			if(resultWait==ETIMEDOUT){
				if(verbose){printf("THREAD CLIENT : TIMER LIMIT\n");}

			}
		}

		argClient->argControler->newThing = 0;
		dataControllerToMessage(sizeFLOAT,message ,argClient->argControler->manette);
		flag=argClient->argControler->manette->flag;
		pthread_mutex_unlock(&argClient->argControler->pmutexReadDataController->mutex);

		if(verbose){printf("THREAD CLIENT SENDING : %s\n", message);}

		message[SIZE_SOCKET_MESSAGE-1]='\0';


		if(sendNetwork(sock,&adr_svr,message)==0){
			//TODO ERROR
		}

		if(flag==0){
			int stopNotReceve = 1;
			while (stopNotReceve) {
				char messageReceve[SIZE_SOCKET_MESSAGE];
				recvfrom(sock, messageReceve, SIZE_SOCKET_MESSAGE, 0, NULL,NULL);//NON BLOCKING

				if (isMessageSTOP(messageReceve) == 1) {
					stopNotReceve = 0;
				} else {
					if(sendNetwork(sock, &adr_svr,message)==0){
						//TODO ERROR
					}
					sleepDuration(1);
				}
			}
			continu=0;
		}
	}

	close(sock);
	return NULL;
}


void *thread_TCP_CLIENT(void *args) {



	args_CLIENT * argClient =(args_CLIENT *) args;
	char verbose=argClient->verbose;

	if(verbose){printf("CLIENT TCP\n");}

	struct sockaddr_in adress_sock;
	adress_sock.sin_family = AF_INET;
	adress_sock.sin_port = htons(argClient->port);
	inet_aton(argClient->adresse, &adress_sock.sin_addr);
	int descr = socket(PF_INET, SOCK_STREAM, 0);

	int r = connect(descr, (struct sockaddr *) &adress_sock,
			sizeof(struct sockaddr_in));
	if (r != -1) {

		int continu = 1;
		char buff[100];
		int size_rec = read(descr, buff, 99 * sizeof(char));
		//TODO do a WHILE SUR le READ
		buff[size_rec] = '\0';

		if(verbose){printf("Message RECU : %s\n", buff);}

		/*
		struct termios oldt, newt;
		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		*/
		const int sizeFLOAT=10;
		char message[5*sizeFLOAT];
		while (continu) {


			struct timespec timeToWait;
			struct timeval now;
			int rt;
			int timeInMs=2000;

			gettimeofday(&now, NULL);

			timeToWait.tv_sec = now.tv_sec + 5;
			timeToWait.tv_nsec = (now.tv_usec + 1000UL * timeInMs) * 1000UL;


			pthread_mutex_lock(&argClient->argControler->pmutexReadDataController->mutex);

			//printf("THREAD CLIENT DU nouveau ?\n");
			int resultWait;
			if(verbose){printf("valeur new ? %d \n",argClient->argControler->newThing);}

			/*
			if(!argClient->argControler->new){
				printf("THREAD CLIENT NON alors j'attends?\n");
				sleep(1);
				resultWait=pthread_cond_timedwait(&argClient->argControler->mutexReadDataController->condition,
						&argClient->argControler->mutexReadDataController->mutex,&timeToWait);
			}

			if(!resultWait){
				if (errno == EAGAIN){
					printf("terminé avant timer");
				}
				message="RIEN";
			}
			*/

			argClient->argControler->newThing=0;
			//printf("THREAD CLIENT OUI alors je regarde?\n");
			dataControllerToMessage(sizeFLOAT,message,argClient->argControler->manette);
			pthread_mutex_unlock(&argClient->argControler->pmutexReadDataController->mutex);

			sleep(1);
			if(verbose){printf("THREAD CLIENT SENDING : %s\n", message);}
			/*


			char str1='X';
			char str2[1];

			int cmp = 0;

			strcpy(str1, (char)'X');
			strcpy(str2, &mess);

			cmp = strcmp(&str1, str2);
			 */
			if (*message=='X') {
				if(verbose){printf("CLIENT EXIT ASK !! \n");}
				continu=0;
			}else{
				int result = write(descr, message, 100);//TODO
				//printf("write is : %d\n", result);
			}
		}
		shutdown(descr, SHUT_RDWR);
		close(descr);
	}else{
		perror("Connection FAIL , drone server not answering");
	}

	return NULL;
}

void *thread_XBOX_CONTROLER(void *args) {

	args_CONTROLER *argClient =(args_CONTROLER *) args;

	control( argClient);

	return NULL;
}


