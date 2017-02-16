#include "concurrent.h"

void init_PMutex(PMutex * arg) {
	if (arg != NULL) {
		arg->mutex = (pthread_mutex_t )PTHREAD_MUTEX_INITIALIZER;
		arg->condition = (pthread_cond_t )PTHREAD_COND_INITIALIZER;
		arg->var = 0;
	}else{
		perror("PMutex not yet malloc\n");
	}
}

void clean_PMutex(PMutex * arg) {
	if (arg != NULL) {
		pthread_mutex_destroy(&arg->mutex);
		pthread_cond_destroy(&arg->condition);
		free(arg);
		arg=NULL;
	}
}


void clean_DataController(DataController * arg){
	if (arg != NULL) {
		clean_PMutex(arg->pmutex);
		free(arg);
		arg=NULL;
	}
}

void sleepDuration(int sleepTime) {
	int result = -1;
	while (result > 0) {
		result = sleep(sleepTime);
		sleepTime -= result;
	}
}

void UsleepDuration(int sleepTime) {
	int result = -1;
	while (result > 0) {
		result = usleep(sleepTime);
		sleepTime -= result;
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
	//Definir la taille de la memoire virtuelle pour que le kernel de fasse pas d'allocation dynamique.
	mlockall(MCL_CURRENT | MCL_FUTURE);

	CPU_ZERO(& cpuset);//mes lensemble a vide
	CPU_SET(id_cpu,& cpuset);//ajoute i a lensemble des CPU
	//fixer l'affinity
	pthread_attr_setaffinity_np( attributs, sizeof(cpu_set_t), & cpuset);

	parametres.sched_priority=priority; //choisir le prioroté (de 0 a 99)
	pthread_attr_setschedpolicy(attributs,SCHED_FIFO);//Inscrire le type d ordonnancement voulu dans les attribue.
	pthread_attr_setschedparam(attributs,&parametres); //incrire la priorite dans les attributs.
	pthread_attr_setinheritsched(attributs,PTHREAD_EXPLICIT_SCHED); //chaque aura sa propre priorité.
	pthread_attr_setscope(attributs,PTHREAD_SCOPE_SYSTEM); // Pour ne pas etre preemte par des processus du system

	#endif

	return 0;
}

int verbose_or_log=0;

void setVerbose(char * verbose, int argc, char * argv,int min) {
	if (argc > min) {
		if (strcmp(argv, "--verbose") == 0) {
			printf("verbose MODE select\n");
			*verbose = 1;
			verbose_or_log=VAL_LOG_VERBOSE;
		}
	} else {
		printf("add    --verbose   for verbose mode\n");
	}
}

void setNoControl(char * NoControl, int argc, char * argv, int min) {
	if (argc > min) {
		if (strcmp(argv, "--noControl") == 0) {
			printf("No controller MODE select\n");
			*NoControl = 1;
		}
	} else {
		printf("add    --noControl  for noController mode\n");
	}
}


void logString(char * str){
	
	if(verbose_or_log==VAL_LOG_FILE){

	}else if(verbose_or_log==VAL_LOG_VERBOSE){
		printf("%s",str);
	}
}
