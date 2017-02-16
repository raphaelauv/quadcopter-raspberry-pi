
void *thread_TCP_CLIENT(void *args) {



	args_CLIENT * argClient =(args_CLIENT *) args;
	//char verbose=argClient->verbose;

	//if(verbose){printf("CLIENT TCP\n");}

	struct sockaddr_in adress_sock;
	adress_sock.sin_family = AF_INET;
	adress_sock.sin_port = htons(UDP_PORT_DRONE);
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

		//if(verbose){printf("Message RECU : %s\n", buff);}

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
			//if(verbose){printf("valeur new ? %d \n",argClient->argControler->newThing);}

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
			//if(verbose){printf("THREAD CLIENT SENDING : %s\n", message);}
			/*


			char str1='X';
			char str2[1];

			int cmp = 0;

			strcpy(str1, (char)'X');
			strcpy(str2, &mess);

			cmp = strcmp(&str1, str2);
			 */
			if (*message=='X') {
				//if(verbose){printf("CLIENT EXIT ASK !! \n");}
				continu=0;
			}else{
				int result = write(descr, message, 100);//TODO
				//printf("write is : %d\n", result);
			}
		}
		shutdown(descr, SHUT_RDWR);
		close(descr);
	}else{
		logString("Connection FAIL , drone server not answering");
	}

	return NULL;
}


void *thread_TCP_SERVER(void *args) {

	args_SERVER *argSERV = (args_SERVER*) args;

	printf("THREAD SERV : SERVEUR TCP\n");

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address_sock;
	address_sock.sin_family = AF_INET;
	address_sock.sin_port = htons(UDP_PORT_DRONE);

	printf("THREAD SERV : attente sur port : %d\n", UDP_PORT_DRONE);

	address_sock.sin_addr.s_addr = htonl(INADDR_ANY);
	int r = bind(sock, (struct sockaddr *) &address_sock,
			sizeof(struct sockaddr_in));
	if (r != 0) {
		return NULL;
	}
	r = listen(sock, 0);
	int connect = 1;
	while (connect) {
		struct sockaddr_in caller;
		socklen_t size = sizeof(caller);

		int sock2 = accept(sock, (struct sockaddr *) &caller, &size);
		printf("THREAD SERV : SERVEUR ACCEPT\n");

		if (sock2 < 0) {
			logString("THREAD SERV : Bind fail");

		}



		pthread_mutex_lock(&argSERV->pmutexRemoteConnect->mutex);

		pthread_cond_signal(&argSERV->pmutexRemoteConnect->condition);

		pthread_mutex_unlock(&argSERV->pmutexRemoteConnect->mutex);


		char *mess = "HELLO\n";
		int result = write(sock2, mess, strlen(mess) * sizeof(char));

		//TODO do a WHILE SUR le WRITE

		fcntl(sock2, F_SETFL, O_NONBLOCK);
		int fd_max = sock2 + 1;
		struct timeval tv;
		fd_set rdfs;

		int fini = 1;
		while (fini) {
			FD_ZERO(&rdfs);
			FD_SET(sock2, &rdfs);
			tv.tv_sec = 3;
			tv.tv_usec = 500000;
			int ret = select(fd_max, &rdfs, NULL, NULL, &tv);
			//printf("valeur de retour de select : %d\n", ret);
			if (ret == 0) {
				printf("THREAD SERV : Timed out\n");
				fini = 0;
			} else if (FD_ISSET(sock2, &rdfs)) {
				int messageRead = 0;
				int iter = 0;
				char buff[SIZE_SOCKET_MESSAGE];
				while (messageRead < 1 && iter < 10) {
					iter++;
					//printf("THREAD SERV : try to read\n");
					int bytesRead = read(sock2, buff, SIZE_SOCKET_MESSAGE - messageRead);
					messageRead += bytesRead;
				}
				if (messageRead > 0) {
					buff[SIZE_SOCKET_MESSAGE-1] = '\0';
					//printf("THREAD SERV : Message recu : %s\n", buff);
					//MessageToStruc(buff, 10, argSERV);

					char str1[2];
					char str2[2];
					int res = 0;
					strcpy(str1, "X\0");
					strcpy(str2, buff);
					res = strcmp(str1, str2);
					if (res == 0) {
						fini = 0;
						printf("THREAD SERV : c'est fini !!\n");
					}
				} else {
					printf("THREAD SERV : NOTHING TO READ !!\n");
					fini = 0;
				}
			} else {
				printf("THREAD SERV : SELECT EXIST WITHOUT GOOD VALUE\n");
			}
		}
		close(sock2);
	}
	return NULL;
}
