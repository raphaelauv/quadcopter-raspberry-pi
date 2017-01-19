#include "client.h"

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

	printf("LE MESSAGE CREER EST : %s\n", output);

	return output;
}


void *thread_TCP_CLIENT(void *args) {

	printf("CLIENT\n");

	 args_CLIENT * argClient =(args_CLIENT *) args;


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

		printf("Message RECU : %s\n", buff);


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
			printf("valeur new ? %d \n",argClient->argControler->newThing);

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
			printf("THREAD CLIENT SENDING : %s\n", message);
			/*


			char str1='X';
			char str2[1];

			int cmp = 0;

			strcpy(str1, (char)'X');
			strcpy(str2, &mess);

			cmp = strcmp(&str1, str2);
			 */
			if (*message=='X') {
				printf("CLIENT EXIT ASK !! \n");
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



int startRemote(char * adresse){

	PMutex * boolControllerPlug =(PMutex *) malloc(sizeof(PMutex));
	init_PMutex(boolControllerPlug);


	PMutex * boolRead =(PMutex *) malloc(sizeof(PMutex));
	init_PMutex(boolRead);

	args_CONTROLER * argControler =(args_CONTROLER *) malloc(sizeof(args_CONTROLER));
	argControler->newThing=0;
	argControler->manette=(DataController *) malloc(sizeof( DataController));
	argControler->pmutexReadDataController=boolRead;
	argControler->pmutexControlerPlug=boolControllerPlug;


	args_CLIENT * argClient =(args_CLIENT *) malloc(sizeof(args_CLIENT));
	argClient->port=8888;
	argClient->adresse=adresse;

	argClient->argControler=argControler;

	pthread_t threadClient;
	pthread_t threadControler;

	printf("TEST MANETTE\n");

	pthread_mutex_lock(&boolControllerPlug->mutex);

	if (pthread_create(&threadControler, NULL, thread_XBOX_CONTROLER, argControler)) {
				perror("pthread_create");
				return EXIT_FAILURE;
	}
	//wait for XBOX CONTROLER


	pthread_cond_wait(&boolControllerPlug->condition, &boolControllerPlug->mutex);

	pthread_mutex_unlock(&boolControllerPlug->mutex);

	printf("MANETTE ok \n");

	//XBOX CONTROLER IS ON , we can start the client socket thread
	if (pthread_create(&threadClient, NULL, thread_TCP_CLIENT, argClient)) {
			perror("pthread_create");
			return EXIT_FAILURE;
	}

	if (pthread_join(threadClient, NULL)){
			perror("pthread_join SERV");
			return EXIT_FAILURE;
	}
	if (pthread_join(threadControler, NULL)){
			perror("pthread_join CONTROLER");
			return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main (int argc, char *argv[]){
	if(argc<2){
		perror("Indiquer l'adresse IP du drone en argument");
		return EXIT_FAILURE;
	}else{
		printf("adresse choisit : %s\n",argv[1]);
		return startRemote(argv[1]);
	}
}
