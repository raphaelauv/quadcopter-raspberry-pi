#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

void * fonction(void * args){
  int num=(int) args;
  while (1){
    printf("[thread %d] je suis sur le CPU %d \n",num,sched_getcpu());
    sleep(5);
  }
}

int main(){
  cpu_set_t cpuset;//ensemble des cpu
  pthread_t thr;
  pthread_attr_t attr; //contien les attribut du nouveaux thread.comme le cpu d'execution
  int i;

  i=0;
  while(1){
    //init avec les attributs par defaut.
    pthread_attr_init(& attr);
    //preparer l'ensemble des CPU
    CPU_ZERO(& cpuset);//mes l'ensemble a vide
    CPU_SET(i,& cpuset);//ajoute i a l'ensemble des CPU
    //fixer l'affinity
    pthread_attr_setaffinity_np(& attr, sizeof(cpu_set_t), & cpuset);
    //lancée le thread
    if(pthread_create(& thr,& attr, fonction,(void *)i) !=0){
      break;
    }
    i++;
  }
  //terminer le thread main en continuant les autre
  pthread_exit(NULL);
}
