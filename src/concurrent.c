#include "concurrent.h"

int init_PMutex(PMutex * arg) {
	if (arg != NULL) {
		arg->mutex = (pthread_mutex_t )PTHREAD_MUTEX_INITIALIZER;
		arg->condition = (pthread_cond_t )PTHREAD_COND_INITIALIZER;

		/*
		arg->var =(volatile int *) malloc(sizeof(int));
		if (arg->var == NULL) {
			logString("MALLOC FAIL : init_PMutex->var");
			return -1;
		}
		*arg->var=0;

		*/

		arg->var=0;
	}else{
		logString("PMutex not yet malloc\n");
		return -1;
	}
	return 0;
}

void clean_PMutex(PMutex * arg) {
	if (arg != NULL) {
		pthread_mutex_destroy(&arg->mutex);
		pthread_cond_destroy(&arg->condition);
		free(arg);
		arg=NULL;
	}
}

void barriereWait(PMutex * Barrier,int size){

	pthread_mutex_lock(&Barrier->mutex);

	(Barrier->var)++;

	while(Barrier->var!=size){
		pthread_cond_wait(&Barrier->condition,
							&Barrier->mutex);
	}
	pthread_cond_signal(&Barrier->condition);

	pthread_mutex_unlock(&Barrier->mutex);
}


void clean_DataController(DataController * arg){
	if (arg != NULL) {
		clean_PMutex(arg->pmutex);
		free(arg);
		arg=NULL;
	}
}

/**
 * if Fail return -1 , else return 0
 */
int init_Attr_Pthread(pthread_attr_t *attributs, int priority,int id_cpu){

	if(pthread_attr_init( attributs)){
		return -1;
	}

	#ifdef __arm__
	cpu_set_t cpuset;

	struct sched_param parametres;
	//Definir la taille de la memoire virtuelle pour que le kernel ne fasse pas d'allocation dynamique.
	mlockall(MCL_CURRENT | MCL_FUTURE);

	CPU_ZERO(& cpuset);//mets l ensemble a vide
	CPU_SET(id_cpu,& cpuset);//ajoute i a l ensemble des CPU
	//fixer l'affinity
	pthread_attr_setaffinity_np( attributs, sizeof(cpu_set_t), & cpuset);

	parametres.sched_priority=priority; //choisir la priorité (de 0 a 99)
	pthread_attr_setschedpolicy(attributs,SCHED_FIFO);//Inscrire le type d ordonnancement voulu dans les attributs.
	pthread_attr_setschedparam(attributs,&parametres); //incrire la priorite dans les attributs.
	pthread_attr_setinheritsched(attributs,PTHREAD_EXPLICIT_SCHED); //chaque aura sa propre priorité.
	pthread_attr_setscope(attributs,PTHREAD_SCOPE_SYSTEM); // Pour ne pas etre preemte par des processus du system

	#endif

	return 0;
}

