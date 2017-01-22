#include "client.h"

int main (int argc, char *argv[]){
	if(argc<2){
		perror("Indiquer l'adresse IP du drone en argument");
		return EXIT_FAILURE;
	}
		printf("adresse choisit : %s\n",argv[1]);
		return startClientRemote(argv[1]);
}
