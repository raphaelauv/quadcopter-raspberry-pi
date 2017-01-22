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


char * dataControllerToMessage(int sizeFloat,DataController * dataController){

	char * output=(char *)malloc(sizeof(char)*(sizeFloat*5));

	int tmp=sizeFloat;

	snprintf(output, sizeFloat, "%f", dataController->moteur0);

	output[tmp-2]=' ';

	snprintf(output+tmp-1, sizeFloat, "%f", dataController->moteur1);

	tmp+=sizeFloat-1;

	output[tmp-2]=' ';

	snprintf(output+tmp-1, sizeFloat, "%f", dataController->moteur2);

	tmp+=sizeFloat-1;

	output[tmp-2]=' ';

	snprintf(output+tmp-1, sizeFloat, "%f", dataController->moteur3);

	//printf("LE MESSAGE CREER EST : %s\n", output);

	return output;
}


char* concat(const char *s1, const char *s2){
    char *result;

    if((result=(char *)malloc(strlen(s1)+strlen(s2)+2))==NULL){
    	return NULL;
    }

    char space[2];
    space[0]=' ';
    space[1]=0;
    strcpy(result, s1);
    strcat(result,(const char *) &space);
    strcat(result, s2);
    return result;
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
		perror("Socket error");
	}

	char str[15];
	sprintf(str, "%d", argClient->port);

	char *myIP=getIP();
	if(myIP!=NULL){
		char * messageWithInfo=concat(myIP,str);
		if(verbose){printf("THREAD CLIENT SENDING : %s\n", messageWithInfo);}
		sendto(sock, messageWithInfo,50, 0, (struct sockaddr *) &adr_svr,sizeof(struct sockaddr_in));

		free(myIP);
	}

	int continu = 1;
	while (continu) {
		char * message;
		pthread_mutex_lock(
				&argClient->argControler->pmutexReadDataController->mutex);

		argClient->argControler->newThing = 0;
		message = dataControllerToMessage(10, argClient->argControler->manette);
		pthread_mutex_unlock(
				&argClient->argControler->pmutexReadDataController->mutex);

		sleep(1);
		if(verbose){printf("THREAD CLIENT SENDING : %s\n", message);}

		/*
		if (*message == 'X') {
			printf("CLIENT EXIT ASK !! \n");
			continu = 0;
		}
		*/

		sendto(sock, message,100, 0, (struct sockaddr *) &adr_svr,sizeof(struct sockaddr_in));

	}
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

		while (continu) {


			char * message;

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
			message=dataControllerToMessage(10,argClient->argControler->manette);
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


int startClientRemote(char * adresse,char verbose){

	PMutex * pmutexControllerPlug =(PMutex *) malloc(sizeof(PMutex));
	init_PMutex(pmutexControllerPlug);


	PMutex * pmutexRead =(PMutex *) malloc(sizeof(PMutex));
	init_PMutex(pmutexRead);

	args_CONTROLER * argControler =(args_CONTROLER *) malloc(sizeof(args_CONTROLER));
	argControler->newThing=0;
	argControler->manette=(DataController *) malloc(sizeof( DataController));
	argControler->pmutexReadDataController=pmutexRead;
	argControler->pmutexControlerPlug=pmutexControllerPlug;
	argControler->verbose=verbose;

	args_CLIENT * argClient =(args_CLIENT *) malloc(sizeof(args_CLIENT));
	argClient->port=8888;
	argClient->adresse=adresse;
	argClient->verbose=verbose;

	argClient->argControler=argControler;

	pthread_t threadClient;
	pthread_t threadControler;

	if(verbose){printf("TEST MANETTE\n");}

	pthread_mutex_lock(&pmutexControllerPlug->mutex);

	if (pthread_create(&threadControler, NULL, thread_XBOX_CONTROLER,argControler)) {
		perror("pthread_create");
		return EXIT_FAILURE;
	}

	//wait for XBOX CONTROLER
	pthread_cond_wait(&pmutexControllerPlug->condition, &pmutexControllerPlug->mutex);

	pthread_mutex_unlock(&pmutexControllerPlug->mutex);

	if(verbose){printf("MANETTE ok \n");}

	//XBOX CONTROLER IS ON , we can start the client socket thread
	if (pthread_create(&threadClient, NULL, thread_UDP_CLIENT, argClient)) {
		perror("pthread_create");
		return EXIT_FAILURE;
	}

	if (pthread_join(threadClient, NULL)) {
		perror("pthread_join SERV");
		return EXIT_FAILURE;
	}
	if (pthread_join(threadControler, NULL)) {
		perror("pthread_join CONTROLER");
		return EXIT_FAILURE;
	}


	clean_args_CLIENT(argClient);
	clean_args_CONTROLER(argControler);

	return EXIT_SUCCESS;
}
