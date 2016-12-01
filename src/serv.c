#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <net/if.h>

void getIP(char * adresse) {

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
		if (addrsTMP->ifa_addr == NULL){
			continue;
		}

		if ((addrsTMP->ifa_flags & IFF_UP) == 0){
			continue;
		}

		if (addrsTMP->ifa_addr->sa_family == AF_INET) {
			s4 = (struct sockaddr_in *) (addrsTMP->ifa_addr);
			if (inet_ntop(addrsTMP->ifa_addr->sa_family, (void *) &(s4->sin_addr),
					ip, 64 * sizeof(char)) != NULL) {


				char str1[15];
				char str2[15];

				int ret=0;

				strcpy(str1, "127.0.0.1");
				strcpy(str2, ip);

				ret = strcmp(str1, str2);

				if(ret!=0){
					printf("Adresse IP :%s\n", ip);
				}

			}
		}
	}
	freeifaddrs(myaddrs);
	free(ip);

}

void TCP(int port) {
	printf("SERVEUR\n");

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address_sock;
	address_sock.sin_family = AF_INET;
	address_sock.sin_port = htons(port);

	printf("attente sur port : %d\n", port);
	address_sock.sin_addr.s_addr = htonl(INADDR_ANY);
	int r = bind(sock, (struct sockaddr *) &address_sock,
			sizeof(struct sockaddr_in));
	if (r == 0) {
		r = listen(sock, 0);
		int connect=1;
		while (connect) {
			struct sockaddr_in caller;
			socklen_t size = sizeof(caller);

			int sock2 = accept(sock, (struct sockaddr *) &caller, &size);
			printf("SERVEUR ACCEPT\n");
			if (sock2 >= 0) {

				int fini = 1;
				do {
					char *mess = "HELLO\n";
					int result = write(sock2, mess,
							strlen(mess) * sizeof(char));

					if (result != 0) {

					}
					int messageRead = 0;

					char buff[100];
					while(messageRead < 1)
					{
					    int bytesRead = read(sock2, buff, 1 - messageRead);
					    messageRead += bytesRead;
					    if(bytesRead==0)
					        fini=0;
					}
					buff[1]='\0';

					printf("Message recu : %s\n", buff);

					char str1[15];
					char str2[15];

					int ret=0;

					strcpy(str1, "END");
					strcpy(str2, buff);

					ret = strcmp(str1, str2);
					if (ret==0) {
						fini = 0;
						printf("c'est fini !!\n");
					}

				} while (fini);
				close(sock2);
			}
		}
	} else {
		printf("Bind fail");
	}
	close(sock);

}


int main() {
	char * adresse = malloc(sizeof(char)*15);
	getIP(adresse);
	TCP(8888);
	return 0;
}
