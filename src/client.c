#include "client.h"

int init_args_CLIENT(args_CLIENT ** argClient,char * adresse,args_CONTROLER * argControler){


	* argClient =(args_CLIENT *) malloc(sizeof(args_CLIENT));
	if (*argClient == NULL) {
		logString("MALLOC FAIL : argClient");
		return EXIT_FAILURE;
	}

	(*argClient)->adresse=adresse;


	(*argClient)->argControler=argControler;

	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		logString("args_CLIENT CLIENT : Socket error");
		return EXIT_FAILURE;
	}

	//adr_my is for reception from drone

	struct sockaddr_in adr_my;
	memset(&adr_my, 0, sizeof(adr_my));
	adr_my.sin_family 		= AF_INET;
	adr_my.sin_addr.s_addr 	= htonl(INADDR_ANY);
	adr_my.sin_port 		= htons(UDP_PORT_REMOTE);


	if(bindUDPSock(&sock,&adr_my)==-1){
		logString("THREAD CLIENT : Socket BIND error");
		close(sock);
		return EXIT_FAILURE;
	}


	if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
		logString("THREAD CLIENT : Socket NONBLOCK error");
		close(sock);
		return EXIT_FAILURE;
	}

	(*argClient)->sock=sock;

	struct sockaddr_in * adr_client=(struct sockaddr_in *) calloc(1,sizeof(struct sockaddr_in));
	if (adr_client == NULL) {
		logString("MALLOC FAIL : argClient->adr_client");
		close(sock);
		return EXIT_FAILURE;
	}
	//memset(adr_client, 0, sizeof(struct sockaddr_in));
	adr_client->sin_family	= AF_INET;
	adr_client->sin_port	= htons(UDP_PORT_DRONE);

	if(inet_aton(adresse, &adr_client->sin_addr)==0){
		logString("FAIL INET_ATON IP ADRESS");
		close(sock);
		return EXIT_FAILURE;
	}
	(*argClient)->adr_client=adr_client;

	return 0;
}

void clean_args_CLIENT(args_CLIENT * arg) {
	if (arg != NULL) {
		clean_PMutex(arg->pmutex);
		//clean_args_CONTROLER(arg->argControler);
		free(arg);
		arg = NULL;
	}
}


void dataControllerToMessage(int sizeFloat,char * output,DataController * dataController){

	snprintf(output,SIZE_MSG_HEADER_DATA,"%s",STR_DATA);

	int tmp=SIZE_MSG_HEADER_DATA+1;

	output[tmp-2]=' ';

	snprintf(output+tmp-1, sizeFloat, "%f", dataController->axe_Rotation);

	tmp+=sizeFloat-1;

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


void concat(const char *typeMsg,const char *s1, const char *s2, char * messageWithInfo){

	/*
	char *result;
    if((result=(char *)malloc(strlen(s1)+strlen(s2)+2))==NULL){
    	return NULL;
    }
	 */
    char space[2];
    space[0]=' ';
    space[1]=0;

    strcpy(messageWithInfo, typeMsg);
    strcat(messageWithInfo,(const char *) &space);
    strcat(messageWithInfo, s1);
    strcat(messageWithInfo,(const char *) &space);
    strcat(messageWithInfo, s2);

    //return result;
}


/**
 *
 * testCloseDrone wait for STOP confirmation from DRONE else resend a STOP MESSAGE
 *
 * if succes return 1 else return 0
 */
int testCloseDrone(int sock,struct sockaddr_in * adr_client , char * message) {
	int stopNotReceve = 1;
	int cmp=0;
	while (stopNotReceve) {
		char messageReceve[SIZE_SOCKET_MESSAGE];
		recvfrom(sock, messageReceve, SIZE_SOCKET_MESSAGE, 0, NULL, NULL); //NON BLOCKING
		cmp++;
		if (isMessageStop(messageReceve) == 1) {
			stopNotReceve = 0;
			return 1;
		}
		else {
			if (sendNetwork(sock, adr_client, message) == -1  || cmp==10) {
				stopNotReceve = 0;
				return 0;
			}
			UsleepDuration(1000000);//TODO
		}
	}
	return 0;
}


void *thread_UDP_CLIENT(void *args) {

	args_CLIENT * argClient = (args_CLIENT *) args;

	logString("THREAD CLIENT : START");

	int sock=argClient->sock;

	struct sockaddr_in * adr_client=argClient->adr_client;

	char str[15];
	sprintf(str, "%d", UDP_PORT_REMOTE);

	char myIP[64];

	//int sizeMessageInfo=15+64;
	char messageWithInfo[SIZE_SOCKET_MESSAGE];


	getIP(myIP);
	if(myIP!=NULL){//TODO
		concat("REMOTE",myIP,str,messageWithInfo);
		messageWithInfo[SIZE_SOCKET_MESSAGE-1]='\0';

		if(sendNetwork(sock,adr_client,messageWithInfo)==-1){
			logString("THREAD CLIENT : SEND NETWORK error");
			//TODO
			return (void*)EXIT_FAILURE;
		}

		//sendto(sock, messageWithInfo,SIZE_SOCKET_MESSAGE, 0, (struct sockaddr *) &adr_svr,sizeof(struct sockaddr_in));
		char array[400];
		sprintf(array,"THREAD CLIENT SENDED : %s", messageWithInfo);
		logString(array);

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
				logString("THREAD CLIENT : TIMER LIMIT");

			}
		}

		argClient->argControler->newThing = 0;
		dataControllerToMessage(sizeFLOAT,message ,argClient->argControler->manette);
		flag=argClient->argControler->manette->flag;
		pthread_mutex_unlock(&argClient->argControler->pmutexReadDataController->mutex);



		message[SIZE_SOCKET_MESSAGE-1]='\0';
		char array[400];
		sprintf(array,"THREAD CLIENT SENDING : %s", message);
		logString(array);

		if(sendNetwork(sock,adr_client,message)==-1){
			//TODO ERROR
		}

		//ARRET
		if(flag==0){

			if(testCloseDrone(sock,adr_client,message)==0) {
				logString("THREAD CLIENT :ERROR message STOP from DRONE NOT RECEVE");
			} else {
				logString("THREAD CLIENT STOP MSG RECEVE FROM DRONE");
			}
			continu=0;
		}
	}

	close(sock);
	logString("THREAD CLIENT : END");
	return NULL;
}


