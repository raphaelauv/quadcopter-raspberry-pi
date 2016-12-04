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
#include <sys/select.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

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
		//TODO do a WHILE SUR le READ
		buff[size_rec] = '\0';

		printf("Message RECU : %s\n", buff);

		char mess;

		struct termios oldt, newt;

		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);

		struct timeval tv;
		fd_set readfds;
		int ret=0;
		while (continu) {
			tv.tv_sec = 0;
			tv.tv_usec = 300000;

			FD_ZERO(&readfds);
			FD_SET(STDIN_FILENO, &readfds);

			// don't care about writefds and exceptfds:
			ret=select(STDIN_FILENO+1 + 1, &readfds, NULL, NULL, &tv);

			if (FD_ISSET(STDIN_FILENO, &readfds)){
				//printf("A key was pressed!\n");
				read(STDIN_FILENO,&mess,1);
				//printf("user enter : %c\n", mess);
			}
			else{
				mess='N';
			}
			printf("SENDING : %c\n", mess);
			/*


			char str1='X';
			char str2[1];

			int cmp = 0;

			strcpy(str1, (char)'X');
			strcpy(str2, &mess);

			cmp = strcmp(&str1, str2);
			 */
			if (mess=='X') {
				printf("CLIENT EXIT ASK !! \n");
				continu=0;
			}else{
				int result = write(descr, &mess, 1);
				//printf("write is : %d\n", result);
			}
		}
		shutdown(descr, SHUT_RDWR);
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

