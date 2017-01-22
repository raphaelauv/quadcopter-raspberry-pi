#include "serv.h"

void clean_args_SERVER(args_SERVER * arg) {
	if (arg != NULL) {
		if (arg->pmutexRemoteConnect != NULL) {
			clean_PMutex(arg->pmutexRemoteConnect);
		}
		free(arg);
		arg=NULL;
	}
}

void getIP() {

	struct ifaddrs *myaddrs, *addrsTMP;
	struct sockaddr_in *s4;
	int status;
	char *ip = (char *) malloc(64 * sizeof(char));
	status = getifaddrs(&myaddrs);
	if (status != 0) {
		perror("Probleme de recuperation d'adresse IP");
		exit(1);
	}
	for (addrsTMP = myaddrs; addrsTMP != NULL; addrsTMP = addrsTMP->ifa_next) {
		if (addrsTMP->ifa_addr == NULL) {
			continue;
		}

		if ((addrsTMP->ifa_flags & IFF_UP) == 0) {
			continue;
		}

		if (addrsTMP->ifa_addr->sa_family == AF_INET) {
			s4 = (struct sockaddr_in *) (addrsTMP->ifa_addr);
			if (inet_ntop(addrsTMP->ifa_addr->sa_family,
					(void *) &(s4->sin_addr), ip, 64 * sizeof(char)) != NULL) {

				char str1[15];
				char str2[15];

				int ret = 0;

				strcpy(str1, "127.0.0.1");
				strcpy(str2, ip);

				ret = strcmp(str1, str2);

				if (ret != 0) {
					printf("Adresse IP :%s\n", ip);
				}

			}
		}
	}
	freeifaddrs(myaddrs);
	free(ip);

}

void MessageToStruc(char * message,int sizeFloat,args_SERVER * arg){



	//PMutex * pmutex = arg->dataController->pmutex->mutex;


	int tmp=sizeFloat;

	float a=strtof(message,0);


	float b=strtof(message+tmp-1,0);

	tmp+=sizeFloat-1;

	float c=strtof(message+tmp-1,0);

	tmp+=sizeFloat-1;

	float d=strtof(message+tmp-1,0);

	pthread_mutex_lock(&arg->dataController->pmutex->mutex);


	arg->dataController->moteur0=a;
	arg->dataController->moteur1=b;
	arg->dataController->moteur2=c;
	arg->dataController->moteur3=d;

	if(arg->dataController->pmutex->var>0){
		arg->dataController->pmutex->var=1;
	}

	pthread_cond_signal(&(arg->dataController->pmutex->condition));

	pthread_mutex_unlock(&(arg->dataController->pmutex->mutex));

	printf ("float a = %.6f  |float b = %.6f  |float c = %.6f  |float d = %.6f  |\n", a ,b,c,d);

}


void *thread_UDP_SERVER(void *args) {

	printf("SERVEUR UDP\n");

	args_SERVER *argSERV = (args_SERVER*) args;

	int sock;
	struct sockaddr_in adr_svr;

	memset(&adr_svr, 0, sizeof(adr_svr));
	adr_svr.sin_family 		= AF_INET;
	adr_svr.sin_addr.s_addr = htonl(INADDR_ANY);
	adr_svr.sin_port 		= htons(8888);

	if((sock=socket(PF_INET,SOCK_DGRAM,0)) ==-1 ){
		perror("Socket error");
	}

	if(bind(sock,(struct sockaddr *)&adr_svr,sizeof(adr_svr))){
		perror("bind error");
	}
	char buff[100];

	recvfrom(sock,buff,99, 0,NULL,NULL);
	printf("messag recu : %s\n",buff);

	pthread_mutex_lock(&argSERV->pmutexRemoteConnect->mutex);
	pthread_cond_signal(&argSERV->pmutexRemoteConnect->condition);
	pthread_mutex_unlock(&argSERV->pmutexRemoteConnect->mutex);

	int fini = 1;
	int i=1;
	while(fini){
		recvfrom(sock,buff,99, 0,NULL,NULL);
		printf("messag recu %d : %s\n",i,buff);
		i++;
	}
}

void *thread_TCP_SERVER(void *args) {
	printf("SERVEUR TCP\n");

	args_SERVER *argSERV = (args_SERVER*) args;

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address_sock;
	address_sock.sin_family = AF_INET;
	address_sock.sin_port = htons(8888);

	printf("attente sur port : %d\n", 8888);

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
		printf("SERVEUR ACCEPT\n");

		if (sock2 < 0) {
			printf("Bind fail");

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
				printf("Timed out\n");
				fini = 0;
			} else if (FD_ISSET(sock2, &rdfs)) {
				int messageRead = 0;
				int iter = 0;
				char buff[100];
				while (messageRead < 1 && iter < 10) {
					iter++;
					//printf("try to read\n");
					int bytesRead = read(sock2, buff, 100 - messageRead);
					messageRead += bytesRead;
				}
				if (messageRead > 0) {
					buff[99] = '\0';
					//printf("Message recu : %s\n", buff);
					MessageToStruc(buff, 10, argSERV);

					char str1[2];
					char str2[2];
					int res = 0;
					strcpy(str1, "X\0");
					strcpy(str2, buff);
					res = strcmp(str1, str2);
					if (res == 0) {
						fini = 0;
						printf("c'est fini !!\n");
					}
				} else {
					printf("NOTHING TO READ !!\n");
					fini = 0;
				}
			} else {
				printf("SELECT EXIST WITHOUT GOOD VALUE\n");
			}
		}
		close(sock2);
	}
	return NULL;
}
