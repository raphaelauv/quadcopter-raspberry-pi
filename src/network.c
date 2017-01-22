#include "network.h"

char* getIP() {

	char * nameIp=(char *)malloc(sizeof(char)*64);
	if(nameIp==NULL){
		return NULL;
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
				strcpy(nameIp,ip);
			}
		}
	}
	freeifaddrs(myaddrs);
	free(ip);
	return nameIp;
}
