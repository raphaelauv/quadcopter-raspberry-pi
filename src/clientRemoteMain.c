#include "client.h"

int main (int argc, char *argv[]){
	if(argc<2){
		perror("Indiquer l'adresse IP du drone en argument");
		return EXIT_FAILURE;
	}
	char verbose=0;
	if(argc>2){
		if(strcmp(argv[2],"--verbose")==0){
			printf("verbose MODE select\n");
			verbose=1;
		}
	}else{
		printf("add    --verbose   for verbose mode\n");
	}
	printf("adresse choisit : %s\n",argv[1]);
	return startClientRemote(argv[1],verbose);
}
