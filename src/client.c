#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>

void TCP(int port, char * adresse) {

	printf("CLIENT\n");
	struct sockaddr_in adress_sock;
	adress_sock.sin_family = AF_INET;
	adress_sock.sin_port = htons(port);
	inet_aton(adresse, &adress_sock.sin_addr);
	int descr = socket(PF_INET, SOCK_STREAM, 0);
	int r = connect(descr, (struct sockaddr *) &adress_sock,
			sizeof(struct sockaddr_in));
	if (r != -1) {

		int continu = 1;
		char buff[100];
		int size_rec = read(descr, buff, 99 * sizeof(char));
		buff[size_rec] = '\0';

		printf("Message RECU : %s\n", buff);

		char mess;
		struct termios oldt, newt;

		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		while (continu) {

			mess = getchar();
			printf("user enter : %c\n", mess);
			char str1[1];
			char str2[1];

			int ret = 0;

			strcpy(str1, "X");
			strcpy(str2, &mess);

			ret = strcmp(str1, str2);

			if (ret == 0) {
				printf("CLIENT EXIT ASK !! \n");
				continu=0;
			}else{
				int result = write(descr, &mess, 1);
				printf("write is : %d\n", result);
			}
		}
		close(descr);
	}

}

int main (int argc, char *argv[]){
	if(argc<2){
		perror("Indiquer l'adresse IP du drone en argument");
		return EXIT_FAILURE;
	}else{
		printf("adresse choisit : %s\n",argv[1]);
		TCP(8888, argv[1]);
		return 0;
	}
}

