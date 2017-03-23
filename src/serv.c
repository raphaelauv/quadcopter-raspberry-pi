#include "serv.h"

int init_args_SERVER(args_SERVER ** argServ){

	PMutex * PmutexDataControler = (PMutex *) malloc(sizeof(PMutex));
	if (PmutexDataControler == NULL) {
		logString("MALLOC FAIL : PmutexDataControler");
		return EXIT_FAILURE;
	}
	init_PMutex(PmutexDataControler);


	DataController * dataControl =(DataController *) malloc(sizeof(DataController));
	if (dataControl == NULL) {
		logString("MALLOC FAIL : dataControl");
		return EXIT_FAILURE;
	}
	dataControl->pmutex=PmutexDataControler;
	dataControl->flag=2;


	PMutex * PmutexRemoteConnect = (PMutex *) malloc(sizeof(PMutex));
	if(PmutexRemoteConnect==NULL){
		logString("MALLOC FAIL : PmutexRemoteConnect");
		return EXIT_FAILURE;
	}
	init_PMutex(PmutexRemoteConnect);


	*argServ =(args_SERVER *) malloc(sizeof(args_SERVER));
	if (*argServ == NULL) {
		logString("MALLOC FAIL : argServ");
		return EXIT_FAILURE;
	}
	(*argServ)->pmutexRemoteConnect = PmutexRemoteConnect;
	(*argServ)->dataController = dataControl;

	(*argServ)->boolStopServ =(volatile int *) malloc(sizeof(int));
	if ((*argServ)->boolStopServ == NULL) {
		logString("MALLOC FAIL : argServ->boolStopServ");
		return -1;
	}
	*(*argServ)->boolStopServ=0;



	int sock;
	struct sockaddr_in adr_svr;
	memset(&adr_svr, 0, sizeof(adr_svr));
	adr_svr.sin_family 		= AF_INET;
	adr_svr.sin_addr.s_addr = htonl(INADDR_ANY);
	adr_svr.sin_port 		= htons(UDP_PORT_DRONE);

	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		logString("THREAD SERV : Socket error");
		return EXIT_FAILURE;
	}
    
	if(bindUDPSock(&sock,&adr_svr) == -1){
		return EXIT_FAILURE;
	}
	(*argServ)->sock=sock;


	return 0;
}


void clean_args_SERVER(args_SERVER * arg) {
	if (arg != NULL) {
		clean_PMutex(arg->pmutexRemoteConnect);
		free(arg);
		arg=NULL;
	}
}

void MessageToStruc(char * message,int sizeFloat,DataController * dataTmp){


	int tmp=SIZE_MSG_HEADER_DATA;

	float a=strtof(message+tmp,NULL);

	tmp+=sizeFloat-1;

	float b=strtof(message+tmp-1,NULL);

	tmp+=sizeFloat-1;

	float c=strtof(message+tmp-1,NULL);

	tmp+=sizeFloat-1;

	float d=strtof(message+tmp-1,NULL);

	tmp+=sizeFloat-1;

	float flagTMP=strtof(message+tmp-1,NULL);

	char flag = (char ) flagTMP;

	dataTmp->axe_Rotation=a;
	dataTmp->axe_UpDown=b;
	dataTmp->axe_LeftRight=c;
	dataTmp->axe_FrontBack=d;
	dataTmp->flag=flag;

}


void unlockWaitMain(args_SERVER *argSERV) {
	pthread_mutex_lock(&argSERV->pmutexRemoteConnect->mutex);
	pthread_cond_signal(&argSERV->pmutexRemoteConnect->condition);
	pthread_mutex_unlock(&argSERV->pmutexRemoteConnect->mutex);
}

/**
 * Manage the new Message receve by the drone
 *
 * return the value of the flag of the message or -1 in case of wrong message
 */
int manageNewMessage(args_SERVER *argSERV,int sock,char * buff,int * cmpNumberMessage,DataController * dataTmp){

	if (receveNetwork(sock, NULL, buff) == -1) {
		//TODO prendre decision sur controleur de vol , demander atterissage
		logString("THREAD SERV : RECEVE NETWORK ERROR");
		return 0;
	}


	buff[SIZE_SOCKET_MESSAGE-1] = '\0';

	char array[SIZE_MAX_LOG];
	sprintf(array, "THREAD SERV : msg receve : %s", buff);
	//logString(array);

	(*cmpNumberMessage)++;

	if(isMessageStop(buff)){

	}
	else if(isMessageRemote(buff)){

	}else if(isMessagePause(buff)){

	}else if(isMessageStop(buff)){

	}else if(isMessageData(buff)){
		MessageToStruc(buff, 10, dataTmp);
	}else{

	}


	if(dataTmp->flag==1){
		logString("THREAD SERV : PAUSE MESSAGE");
		return 1;
	}

	else {

		/*
		if(argSERV->verbose){
			printf ("THREAD SERV : float a = %.6f  |float b = %.6f  |float c = %.6f  |float d = %.6f  | FLAG = %d\n",
					dataTmp->axe_Rotation ,dataTmp->axe_UpDown,dataTmp->axe_LeftRight,dataTmp->axe_FrontBack,dataTmp->flag);
		}
		*/


		pthread_mutex_lock(&argSERV->dataController->pmutex->mutex);

		argSERV->dataController->axe_Rotation=dataTmp->axe_Rotation;
		argSERV->dataController->axe_UpDown=dataTmp->axe_UpDown;
		argSERV->dataController->axe_LeftRight=dataTmp->axe_LeftRight;
		argSERV->dataController->axe_FrontBack=dataTmp->axe_FrontBack;
		argSERV->dataController->flag=dataTmp->flag;
		argSERV->dataController->pmutex->var=1;

		pthread_cond_signal(&(argSERV->dataController->pmutex->condition));

		pthread_mutex_unlock(&(argSERV->dataController->pmutex->mutex));

		if (dataTmp->flag == 0) {

			logString("THREAD SERV : FLAG 0 MESSAGE");

			//fini = 0;
			return 0;
		}
		return 2;
	}

	/*
	else{
		logString("THREAD SERV : UNKNOW MESSAGE\n");
		return 0;
	}

	*/
}

void *thread_UDP_SERVER(void *args) {

	args_SERVER *argSERV = (args_SERVER*) args;

	logString("THREAD SERV : SERVEUR UDP");
	int sock=argSERV->sock;

	struct sockaddr_in adr_send;
	int runServ = 1;
	int socketConnectionMade=0;

	/*********************************************************************/
	/*				FIRST MESSAGE WITH UDP INFO FROM REMOTE*/
	/*********************************************************************/

	char buff[SIZE_SOCKET_MESSAGE];
	int cmpNumberMessage = 1;

	int notConnected=1;
	while(notConnected && runServ){
		if (receveNetwork(sock, NULL, buff) == -1) {//TODO attente borné
				logString("THREAD SERV :FAIL RECEVE NETWORK ERROR");
				runServ=0;//TODO
				notConnected=0;
		}

		buff[SIZE_SOCKET_MESSAGE - 1] = '\0';
		char array[SIZE_MAX_LOG];
		sprintf(array,"THREAD SERV : mesg receve : %s", buff);
		logString(array);
		memset(&adr_send, 0, sizeof(adr_send));
		adr_send.sin_family = AF_INET;

		if (get_IP_Port(buff, &adr_send) != 1) {
			logString("ERROR IP RECEVE");
		} else {
			logString("THREAD SERV : GOOD IP AND PORT RECEVE");
			socketConnectionMade = 1;
			notConnected=0;
		}
	}


	/*********************************************************************/
	/*				SELECT ON UDP SOCKET*/
	/*********************************************************************/

	DataController dataTmp;
	int firstMessage=1;

	fcntl(sock, F_SETFL, O_NONBLOCK);
	int fd_max = sock + 1;
	struct timeval tv;
	fd_set rdfs;

	int counterMsg=0;
	int freqConfirm=(int)FREQUENCY_CONTROLLER;

	
	while(runServ && !(*(argSERV->boolStopServ))){

		FD_ZERO(&rdfs);
		FD_SET(sock, &rdfs);
		tv.tv_sec = UDP_TIME_SEC_TIMER;
		tv.tv_usec = UDP_TIME_USEC_TIMER;
		int ret = select(fd_max, &rdfs, NULL, NULL, &tv);
		if (ret == 0) {
			logString("THREAD SERV : Timed out");
			//TODO
			//fini = 0;
		} else if (FD_ISSET(sock, &rdfs)) {

			if (firstMessage) {
				unlockWaitMain(argSERV);
				firstMessage = 0;
			}

			counterMsg++;
			if(counterMsg%freqConfirm==0){
				char confirm[8] = "CONFIRM";
				if (sendNetwork(sock, &adr_send, confirm) == -1) {
					logString("THREAD SERV : NETWORK ERROR DURING -> SEND CONFIRM");
					//TODO
				}
			}

			if(manageNewMessage(argSERV,sock,buff,&cmpNumberMessage,&dataTmp)==0){
				runServ=0;
			}

		} else {
			logString("THREAD SERV : SELECT WITHOUT GOOD VALUE");
		}
	}

	/*********************************************************************/
	/*				CLOSE CONNECTION AND THREAD							 */
	/*********************************************************************/


	if (socketConnectionMade) {
		/*
		 * Code without any receve security ( and no need for )
		 * NO CRITIC SECTION
		 */
		//char stop[5] = "STOP";
		char stop[SIZE_MSG_HEADER_STOP+1] = STR_STOP;
		logString("THREAD SERV : SEND STOP");
		for (int i = 0; i < 10; i++) {
			if (sendNetwork(sock, &adr_send, stop) == -1) {
				logString("THREAD SERV : NETWORK ERROR DURING -> SEND STOP");
				break;
			}
		}

	}

	close(sock);

	//Pour debloquer attente dans main si
	unlockWaitMain(argSERV);

	logString("THREAD SERV : END");

	return NULL;
}

