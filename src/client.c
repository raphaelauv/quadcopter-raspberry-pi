#include "client.h"
#include "Manette/controller.h"

void *thread_TCP_CLIENT(void *args) {

	args_CLIENT *argClient = args;

	printf("CLIENT\n");
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
	}else{
		perror("Connection FAIL , drone server not answering");
	}

}

void *thread_XBOX_CONTROLER(void *args) {
	struct controller manette;
	controller( &manette);
}


int startRemote(char * adresse){
	pthread_t threadClient;
	pthread_t threadControler;

	args_CLIENT * argClient = malloc(sizeof(args_CLIENT));
	argClient->port=8888;
	argClient->adresse=adresse;


	if (pthread_create(&threadClient, NULL, thread_TCP_CLIENT, argClient)) {
			perror("pthread_create");
			return EXIT_FAILURE;
	}
	if (pthread_create(&threadControler, NULL, thread_XBOX_CONTROLER, NULL)) {
			perror("pthread_create");
			return EXIT_FAILURE;
	}

	if (pthread_join(threadClient, NULL)){
			perror("pthread_join");
			return EXIT_FAILURE;
	}
	if (pthread_join(threadControler, NULL)){
			perror("pthread_join");
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
