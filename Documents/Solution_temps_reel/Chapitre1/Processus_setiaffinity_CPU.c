#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

int main(int args,char *argv[]){
	cpu_set_t cpuset; //ensemble de cpu utilisable par le processus
	int cpu;
	if((args != 2) || (sscanf(argv[1],"%d", &cpu)!=1)){
		fprintf(stderr,"usage: %s cpu\n,argv[0]");
		exit(1);
	}
	CPU_ZERO(& cpuset);//vide l'ensemble
	CPU_SET(cpu,&cpuset);//ajoute un element.
	//CPU_ISSET(cpu,cpuset); pour enlever un element
	if(sched_setaffinity(0,sizeof(cpuset),&cpuset)!=0){
		perror(argv[1]);
		exit(1);
	}
	while(1){
		printf("Je suis sur le CPU %d\n",sched_getcpu());
	}
	return 0;
}
