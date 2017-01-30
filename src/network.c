#include "network.h"


char isMessage(char * messageReceve, char * messageToTest) {
	char str1[SIZE_SOCKET_MESSAGE];
	char str2[SIZE_SOCKET_MESSAGE];
	int res = 0;

	strcpy(str1, messageToTest);
	strcpy(str2, messageReceve);
	res = strcmp(str1, str2);

	return res == 0;
}

char isMessagePause(char * message) {
	return isMessage(message,"PAUSE");
}

char isMessageSTOP(char * message){
	return isMessage(message,"STOP");
}


int bindUDPSock(int * sock, struct sockaddr_in * adr_svr) {

	int enable = 1;

	if (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
		return 0;
	}

	if (bind(*sock, (struct sockaddr *) adr_svr, sizeof(*adr_svr))) {
		perror("bind error");
		return 0;
	}
	return 1;
}

int receveNetwork(int sock, struct sockaddr_in *adr_svr, char * message) {
	int sizeReveceTotal = 0;
	int resultReceve=0;
	int nbfail = 0;

	while (sizeReveceTotal < SIZE_SOCKET_MESSAGE && nbfail < 10) {
		resultReceve = recvfrom(sock, &message[sizeReveceTotal],
				SIZE_SOCKET_MESSAGE - sizeReveceTotal, 0, NULL, NULL);

		if (resultReceve == -1) {
			nbfail++;
		} else {
			sizeReveceTotal += resultReceve;
		}
	}
	if (nbfail == 10) {
		return 0;
	} else {
		return 1;
	}

}
int sendNetwork(int sock,struct sockaddr_in *adr_svr,char * message) {
	int sended=0;
	int resultSend=0;
	int nbfail=0;
	while (sended < SIZE_SOCKET_MESSAGE && nbfail<10) {
		resultSend = sendto(sock, &message[sended],
			SIZE_SOCKET_MESSAGE - sended, 0, (struct sockaddr *) adr_svr,
				sizeof(struct sockaddr_in));


		if (resultSend == -1) {
			nbfail++;
		}else{
			sended += resultSend;
		}
	}
	if(nbfail==10){
		return 0;
	}else{
		return 1;
	}
}

void getIP(char*  myIP) {
	//char * nameIp=(char *)malloc(sizeof(char)*64);
	if(myIP==NULL){
		return;
	}
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
				strcpy(myIP,ip);
			}
		}
	}
	freeifaddrs(myaddrs);
	free(ip);
	//return nameIp;
}
