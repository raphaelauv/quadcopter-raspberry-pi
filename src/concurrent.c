#include "concurrent.h"

void init_mask(void (*functionPtr)(int)){
  int rc;

  /* for the moment sigaction for SIGINT signal only*/
  struct sigaction sa;
  memset(&sa,0,sizeof(sa));
  sa.sa_flags = 0;
  sa.sa_handler = functionPtr;
  /* mask all the other signal while SIGINT signal is catched */
  sigset_t set;
  rc = sigfillset(&set);
  ERROR(rc, "sigaddset\n");

  sa.sa_mask = set;

  rc = sigaction(SIGINT, &sa, NULL);
  ERROR(rc,"sigaction\n");

}


int init_PMutex(PMutex * arg) {
	if (arg != NULL) {
		arg->mutex = (pthread_mutex_t )PTHREAD_MUTEX_INITIALIZER;
		arg->condition = (pthread_cond_t )PTHREAD_COND_INITIALIZER;
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
int init_Attr_Pthread(pthread_attr_t *attributs, int priority,int id_cpu,void *stack_buf){



	if (pthread_attr_init(attributs)) {
		logString("init_Attr_Pthread : ERROR pthread_attr_init failed");
		return -1;
	}

	#ifdef __arm__
	char arrayStr[SIZE_MAX_LOG];
	cpu_set_t cpuset;
	struct sched_param parametres;

	//Definir la taille de la memoire virtuelle pour que le kernel ne fasse pas d'allocation dynamique.
	if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
		sprintf(arrayStr,"init_Attr_Pthread : ERROR mlockall failed : %s", strerror(errno));
		logString(arrayStr);
		return -1;
	}


	/* TODO
	stack_buf = mmap(NULL, PTHREAD_STACK_MIN, PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (stack_buf == MAP_FAILED) {
		sprintf(arrayStr,"init_Attr_Pthread : ERROR mmap failed : %s", strerror(errno));
		logString(arrayStr);
		return -1;
	}

	memset(stack_buf, 0, PTHREAD_STACK_MIN);

	if(pthread_attr_setstack(attributs, stack_buf, PTHREAD_STACK_MIN)){
		sprintf(arrayStr,"init_Attr_Pthread : ERROR setstack failed : %s", strerror(errno));
		logString(arrayStr);
		return -1;
	}
	*/

	//Inscrire le type d ordonnancement voulu dans les attributs.
	if(pthread_attr_setschedpolicy(attributs,SCHED_FIFO)){
		sprintf(arrayStr,"init_Attr_Pthread : ERROR setschedpolicy failed : %s", strerror(errno));
		logString(arrayStr);
		return -1;
	}

	CPU_ZERO(& cpuset);//mets l ensemble a vide
	CPU_SET(id_cpu,& cpuset);//ajoute i a l ensemble des CPU

	//fixer l'affinity
	if(pthread_attr_setaffinity_np( attributs, sizeof(cpu_set_t), & cpuset)){
		sprintf(arrayStr,"init_Attr_Pthread : ERROR setaffinity_np failed : %s", strerror(errno));
		logString(arrayStr);
		return -1;
	}

	if (priority < 0 || priority > 99) {
		logString("init_Attr_Pthread : ERROR priority argument out of bounds");
		return -1;
	}
	parametres.sched_priority=priority; //choisir la priorité (de 0 a 99)

	//incrire la priorite dans les attributs.
	if(pthread_attr_setschedparam(attributs,&parametres)){
		sprintf(arrayStr,"init_Attr_Pthread : ERROR setschedparam failed : %s", strerror(errno));
		logString(arrayStr);
		return -1;
	}

	//chaque aura sa propre priorité.
	if(pthread_attr_setinheritsched(attributs,PTHREAD_EXPLICIT_SCHED)){
		sprintf(arrayStr,"init_Attr_Pthread : ERROR setinheritsched failed : %s", strerror(errno));
		logString(arrayStr);
		return -1;
	}
	 // Pour ne pas etre preemte par des processus du system
	if(pthread_attr_setscope(attributs,PTHREAD_SCOPE_SYSTEM)){
		sprintf(arrayStr,"init_Attr_Pthread : ERROR setscope failed : %s", strerror(errno));
		logString(arrayStr);
		return -1;
	}


	#endif

	return 0;
}

