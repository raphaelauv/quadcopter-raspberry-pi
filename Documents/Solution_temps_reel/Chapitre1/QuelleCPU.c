
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(){
	int n;
	int precedent=-1;
	time_t heure;
	struct tm * tm_heure;
	while(1){
		n=sched_getcpu();
		if(n==-1){
			perror("sched_getcpu");
			exit(1);
		}
		if(n!=precedent){
                        heure=time(NULL);
                        tm_heure=localtime(&heure);
                        printf("%02d:%02d:%02d migration %d -> %d \n",tm_heure->tm_hour,tm_heure->tm_min,tm_heure->tm_sec,precedent,n);
			precedent=n;
                }

		if(precedent==-1){
			precedent=n;
		}
	}
	return 0;
}

