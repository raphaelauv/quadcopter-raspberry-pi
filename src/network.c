#include "network.h"

/**
 * Fill sa ( sockaddr_in ) with the ip and number port inside the message
 *
 * return 0 if FAIL ,else 1
 */
char get_IP_Port(char *message,struct sockaddr_in * sa){

	if(isMessageRemote(message)){
		message+=SIZE_MSG_HEADER_REMOTE;
	}else{
		return 0;
	}

	char ip[50];
	int cmp=0;
	while(*message!=' '){
		ip[cmp]=*message;
		message++;
		cmp++;
	}

	message++;
	int cmp2=0;

	char port[5];

	while(*message!='\0'){
		port[cmp2]=*message;
		message++;
		cmp2++;
	}

	int nbPort = atoi(port);
	if(nbPort>0 && nbPort<9999){
		sa->sin_port=htons(nbPort);
	}else{
		return 0;
	}

	cmp++;
	ip[cmp]='\0';

	//printf("ip get : %s\n",ip);
	return inet_pton(AF_INET,(const char *) &ip, &(sa->sin_addr));
}

char isMessage(char * messageReceve, char * messageToTest,int nb) {
	char str1[SIZE_SOCKET_MESSAGE];
	char str2[SIZE_SOCKET_MESSAGE];
	int res = 0;

	strcpy(str1, messageToTest);
	strcpy(str2, messageReceve);

	if(nb>0){
		res = strncmp(str1, str2, nb);
	}else{
		res = strcmp(str1, str2);
	}
	return res == 0;
}

char isMessageRemote(char * message){
	char msg[SIZE_MSG_HEADER_REMOTE+1] = STR_REMOTE;
	return isMessage(message,msg,SIZE_MSG_HEADER_REMOTE);
}
char isMessageData(char * message){
	char msg[SIZE_MSG_HEADER_DATA+1] = STR_DATA;
	return isMessage(message,msg,SIZE_MSG_HEADER_DATA);
}

char isMessagePause(char * message) {
	char msg[SIZE_MSG_HEADER_PAUSE+1] = STR_PAUSE;
	return isMessage(message,msg,0);
}

char isMessageStop(char * message){
	char msg[SIZE_MSG_HEADER_STOP+1] = STR_STOP;
	return isMessage(message,msg,0);
}


/**
 * Bind the sock with adr_svr information and set it SO_REUSEADDR
 *
 * return 0 if FAIL , else 1
 */
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


/**
 * Fill message with the receved message , message size should be at least SIZE_SOCKET_MESSAGE
 * Fill adr_svr with the receved Info
 *
 * Return 0 in case of Error else 1
 */
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

/**
 * Send message to adr_svr , message size should be at least SIZE_SOCKET_MESSAGE
 *
 * Return 0 in case of Error else 1
 */
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

/**
 * Fill myIP with an IP availalble on the network interface
 * myIP need to be malloc of a size of 64 char
 *
 */
void getIP(char*  myIP) {
	if(myIP==NULL){
		return;
	}
	struct ifaddrs *myaddrs, *addrsTMP;
	struct sockaddr_in *s4;
	int status;
	/*
	char *ip = (char *) malloc(64 * sizeof(char));
	if(ip==NULL){
		return;
	}
	*/
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
					(void *) &(s4->sin_addr), myIP, 64 * sizeof(char)) != NULL) {

				char str1[15];
				char str2[15];

				int ret = 0;

				strcpy(str1, "127.0.0.1");
				strcpy(str2, myIP);

				ret = strcmp(str1, str2);

				if (ret != 0) {
					printf("Adresse IP :%s\n", myIP);
				}
				//strcpy(myIP,ip);
			}
		}
	}
	freeifaddrs(myaddrs);
	//free(ip);
	//return nameIp;
}
