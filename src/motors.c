#include "motors.h"
#include "concurrent.h"

double frequence=50.0; //frequence du signal d'entré des ESCs
double periode=0; // periode = 1/frequence. Initialisée plus tard.

// Pointeur de fonction qui controle chaque ESC


void clean_motorsAll(MotorsAll * arg) {
	if (arg != NULL) {
		if (arg->bool_arret_moteur != NULL) {
			free((void*)arg->bool_arret_moteur);
		}
		clean_motor_info(arg->motor0);
		clean_motor_info(arg->motor1);
		clean_motor_info(arg->motor2);
		clean_motor_info(arg->motor3);
		free(arg);
		arg = NULL;
	}
}

void clean_motor_info(motor_info * arg){
	if (arg != NULL) {
		clean_PMutex(arg->MutexSetPower);
		if(arg->bool_arret_moteur!=NULL){
			free((void*)arg->bool_arret_moteur);
			arg->bool_arret_moteur=NULL;
		}
		free(arg);
		arg=NULL;
	}
}



void * thread_startMoteur(void * args){

	if(args==NULL){
		perror("args thread_startMoteur is NULL\n");
	}

    motor_info * info=(motor_info *)args;

    printf("THREAD MOTOR INIT-> %d \n",info->broche);

    int low,hight;

	#ifdef __arm__

    if (wiringPiSetup () == -1){
        return NULL;
    }
	pinMode (info->broche, OUTPUT); //On defini la sorti du signal
	#endif


    int runMotor=1;
    while(runMotor){
    	sleep(5);


        //On Bloc le Mutex, on copie les valeurs info->high_time et info->low_time pour pas resté avec le mutex bloquée.
    	pthread_mutex_lock(&info->MutexSetPower->mutex);
        if(!(*(info->bool_arret_moteur))){//Dans le cas on est pas dans une demande d'arret moteur.
        	hight=(int)info->high_time;
        	low=(int)info->low_time;

        	//printf("valeur %d de bool arret %d\n",info->broche,*info->bool_arret_moteur);
        	//printf("THREAD MOTOR -> %d  | HIGH %d  LOW %d \n",info->broche, hight, low);

        	pthread_mutex_unlock(&info->MutexSetPower->mutex);
			#ifdef __arm__
        	digitalWrite(info->broche, 1);       // Etat haut du signal.
			#endif
        	usleep((int)hight);
			#ifdef __arm__
        	//digitalWrite(info->broche,0);         //Etat bas du signal.
			#endif
        	usleep((int)(low));


        	//printf("DANS BRANCH MOTOR %d  : valeur HIGH -> %f  valeur LOW -> %f\n",info->broche, info->high_time,info->low_time);
        }
        else{//ARRET des moteurs d'urgence demandé.
        	//printf("THREAD MOTOR : ARRET %d\n",info->broche);
        	//printf("valeur %d de bool arret %d\n",info->broche,*info->bool_arret_moteur);
        	runMotor=0;
        }
    }


    return NULL;
}

char init_motor_info(motor_info *info,int broche,volatile int * stop){

    if(periode<=0){//Si la periode n'est pas Initialisé
        perror("Fatal erreur:Periode don't initialize\n");
        return 0;
    }

    if(info==NULL){
    	perror("info is NULL");
    	return 0;
    }


    info->bool_arret_moteur=stop;
    info->broche=broche;
    info->high_time=(periode*5.0/100.0);;//Correspond a 0% de puissance .(1/Frequence * 5/100)= 1 dans notre cas.
    info->low_time=periode-(info->high_time);// le reste de la periode.

    PMutex * MutexSetPower=(PMutex *)malloc(sizeof(PMutex));
    if(MutexSetPower ==NULL){
    	perror("MALLOC FAIL : init_motor_info - MutexSetPower\n");
    	return 0;
    }
    init_PMutex(MutexSetPower);
    info->MutexSetPower=MutexSetPower;
    return 1;

}

int set_power(motor_info * info,float power){
    int a=(int)power;

    if( (a>0 && a<5) || a>10){
        return 1;
    }

    //printf("THREAD CONTROLVOL : SET POWER avant lock\n");
    pthread_mutex_lock(&info->MutexSetPower->mutex);

    //printf("THREAD CONTROLVOL : SET POWER dans lock\n");

    if(a==0 && *info->bool_arret_moteur==1){
    	//TODO arret moteur
    }

    info->high_time=(periode*power/100.0); // On calcule le nouveaux rapport cyclique.
    info->low_time=periode-info->high_time; //
    pthread_mutex_unlock(&info->MutexSetPower->mutex);

    //printf("THREAD CONTROLVOL : SET POWER apres lock\n");
    //printf("%i\n",a);
    return 0;
}


char init_Value_motors(MotorsAll * motorsAll){

    //init 0% de puissance des moteur en fonction de la frequence
    periode=(1.0/frequence)*1000000;


	int m0,m1,m2,m3;
	//init broche du signal du controle des moteur
	m0=5;
	m1=28;
	m2=2;
	m3=24;

	motor_info * info_m0 =(motor_info *) malloc(sizeof(motor_info));
	motor_info * info_m1 =(motor_info *) malloc(sizeof(motor_info));
	motor_info * info_m2 =(motor_info *) malloc(sizeof(motor_info));
	motor_info * info_m3 =(motor_info *) malloc(sizeof(motor_info));

	if(info_m0==NULL || info_m1==NULL || info_m2==NULL || info_m3==NULL){
		perror("MALLOC FAIL : init_Value_motors - motor_info\n");
		return 0;
	}

	motorsAll->motor0 = info_m0;
	motorsAll->motor1 = info_m1;
	motorsAll->motor2 = info_m2;
	motorsAll->motor3 = info_m3;

	int returnofINITS=0;

	returnofINITS+=init_motor_info(motorsAll->motor0,m0,motorsAll->bool_arret_moteur);
	returnofINITS+=init_motor_info(motorsAll->motor1,m1,motorsAll->bool_arret_moteur);
	returnofINITS+=init_motor_info(motorsAll->motor2,m2,motorsAll->bool_arret_moteur);
	returnofINITS+=init_motor_info(motorsAll->motor3,m3,motorsAll->bool_arret_moteur);

	if(returnofINITS!=4){
		return 0;
	}
    return 1;

}

void init_threads_motors(MotorsAll * motorsAll,char verbose){
    cpu_set_t cpuset;//ensemble des CPU utilisable.

    pthread_t thr0;
    pthread_t thr1;
    pthread_t thr2;
    pthread_t thr3;
    pthread_attr_t attributs;

    struct sched_param parametres;

    //init avec les attributs par defaut.
    pthread_attr_init(& attributs);





	#ifdef __arm__

    //Definir la taille de la memoire virtuelle pour que le kernel de fasse pas d'allocation dynamique.
    //mlockall(MCL_CURRENT | MCL_FUTURE);
    

    //Mettre Sur un coeur ici le coeur 0;
    CPU_ZERO(& cpuset);//mes lensemble a vide
    CPU_SET(0,& cpuset);//ajoute i a lensemble des CPU
    //fixer l'affinity
    pthread_attr_setaffinity_np(& attributs, sizeof(cpu_set_t), & cpuset);
    
    //Priorite temps reel.
    parametres.sched_priority=99; //choisir le prioroté (de 0 a 99)
    pthread_attr_setschedpolicy(&attributs,SCHED_FIFO);//Inscrire le type d ordonnancement voulu dans les attribue.
    pthread_attr_setschedparam(&attributs,&parametres); //incrire la priorite dans les attributs.
    pthread_attr_setinheritsched(&attributs,PTHREAD_EXPLICIT_SCHED); //chaque aura sa propre priorité.
    pthread_attr_setscope(&attributs,PTHREAD_SCOPE_SYSTEM); // Pour ne pas etre preemte par des processus du system
	#endif


    //creation du thread.
	if (pthread_create(&thr0, &attributs, thread_startMoteur, motorsAll->motor0)) {
		perror("pthread0_create");
	}

	if (pthread_create(&thr1, &attributs, thread_startMoteur, motorsAll->motor1)) {
		perror("pthread1_create");
	}

	if (pthread_create(&thr2, &attributs, thread_startMoteur, motorsAll->motor2)) {
		perror("pthread2_create");
	}

	if (pthread_create(&thr3, &attributs, thread_startMoteur, motorsAll->motor3)) {
		perror("pthread3_create");
	}

	pthread_attr_destroy(&attributs);//Libere les resource.

	if (pthread_join(thr0, NULL)) {
		perror("pthread_join");
	}
	if (pthread_join(thr1, NULL)) {
		perror("pthread_join");
	}
	if (pthread_join(thr2, NULL)) {
		perror("pthread_join");
	}
	if (pthread_join(thr3, NULL)) {
		perror("pthread_join");
	}

	if(verbose){printf("THREADS 4 MOTORS : END\n");}

}
