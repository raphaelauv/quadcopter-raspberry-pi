#include "motors.h"
#include "concurrent.h"

double frequence=50.0; //frequence du signal d'entré des ESCs
double periode=0; // periode = 1/frequence. Initialisée plus tard.

// Pointeur de fonction qui controle chaque ESC


int init_MotorsAll(MotorsAll ** motorsAll){
	*motorsAll =(MotorsAll *) malloc(sizeof(MotorsAll));
	if (*motorsAll == NULL) {
		logString("MALLOC FAIL : motorsAll\n");
		return -1;
	}
	(*motorsAll)->bool_arret_moteur =(volatile int *) malloc(sizeof(int));

	if ((*motorsAll)->bool_arret_moteur == NULL) {
		logString("MALLOC FAIL : motorsAll->bool_arret_moteur\n");
		return -1;
	}
	volatile int arret=0;//TODO
	(*(*motorsAll)->bool_arret_moteur)= arret;


	PMutex * barrier = (PMutex *) malloc(sizeof(PMutex));
	if (barrier == NULL) {
		logString("MALLOC FAIL : barrier");
		return -1;
	}
	init_PMutex(barrier);

	(*motorsAll)->Barrier=barrier;

	if(init_Value_motors(*motorsAll)){
		return -1;
	}

	return 0;
}

void clean_Motor_info(Motor_info * arg){
	if (arg != NULL) {
		clean_PMutex(arg->MutexSetPower);
		/*
		if(arg->bool_arret_moteur!=NULL){
			free((int*)arg->bool_arret_moteur);
			arg->bool_arret_moteur=NULL;
		}
		*/
		free(arg);
		arg=NULL;
	}
}

void clean_MotorsAll(MotorsAll * arg) {
	if (arg != NULL) {
		if (arg->Barrier != NULL) {
			free(arg->Barrier);
			arg->Barrier = NULL;
		}

		if (arg->bool_arret_moteur != NULL) {
			free((void *)arg->bool_arret_moteur);
			arg->bool_arret_moteur=NULL;
		}
		for(int i=0;i<NUMBER_OF_MOTORS;i++){
			clean_Motor_info(arg->arrayOfMotors[i]);
		}

		free(arg->arrayOfMotors);
		free(arg);
		arg = NULL;
	}
}



void * thread_startMoteur(void * args){

	Motor_info * info=(Motor_info *)args;

    int low,hight;

	#ifdef __arm__

    if (wiringPiSetup () == -1){
        return NULL;
    }
	pinMode (info->broche, OUTPUT); //On defini la sorti du signal
	#endif

	char array[400];
	sprintf(array,"THREAD MOTOR %d : INIT DONE",info->broche);
    logString(array);


	sprintf(array,"THREAD MOTOR %d : BARRIER PASS",info->broche);
	barriereWait(info->Barrier,NUMBER_OF_MOTORS);
	logString(array);

    int runMotor=1;
    while(runMotor){
    	//sleep(5);

        //On Bloc le Mutex, on copie les valeurs info->high_time et info->low_time pour pas resté avec le mutex bloquée.
    	pthread_mutex_lock(&info->MutexSetPower->mutex);

        if((*(info->bool_arret_moteur))!=1){//Dans le cas on est pas dans une demande d'arret moteur.
        	hight=(int)info->high_time;
        	low=(int)info->low_time;

        	//printf("valeur %d de bool arret %d\n",info->broche,*info->bool_arret_moteur);
        	//printf("THREAD MOTOR -> %d  | HIGH %d  LOW %d \n",info->broche, hight, low);

        	pthread_mutex_unlock(&info->MutexSetPower->mutex);
			#ifdef __arm__
        	digitalWrite(info->broche,1);       // Etat haut du signal.
			#endif
        	usleep((int)hight);
			#ifdef __arm__
        	digitalWrite(info->broche,0);         //Etat bas du signal.
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

    sprintf(array,"THREAD MOTOR %d : END",info->broche);
    logString(array);

    return NULL;
}

/*
 * Return -1 is FAIL else return 0 in SUCCES
 */
int init_Motor_info(Motor_info *info,int broche,volatile int * stop,PMutex * barrier){

    if(periode<=0){//Si la periode n'est pas Initialisé
    	logString("FAIL : Periode not initialize\n");
        return -1;
    }

    if(info==NULL){
    	logString("FAIL : init_Motor_info-> info is NULL");
    	return -1;
    }


    info->bool_arret_moteur=stop;
    info->broche=broche;
    info->high_time=(periode*5.0/100.0);;//Correspond a 0% de puissance .(1/Frequence * 5/100)= 1 dans notre cas.
    info->low_time=periode-(info->high_time);// le reste de la periode.

    PMutex * MutexSetPower=(PMutex *)malloc(sizeof(PMutex));
    if(MutexSetPower ==NULL){
    	logString("MALLOC FAIL : init_Motor_info - MutexSetPower\n");
    	return -1;
    }
    init_PMutex(MutexSetPower);
    info->MutexSetPower=MutexSetPower;
    info->Barrier=barrier;
    return 0;

}

int set_power(Motor_info * info,float power){
    int powerVerified=(int)power;


	//printf("\n BROCHE : %d  | POWER : %f  \n",info->broche,power);
    if( (powerVerified>0 && powerVerified<5) || powerVerified>10){
        return -1;
    }

    if(powerVerified>10){
    	powerVerified=10;
    }else if(powerVerified<5){
    	powerVerified=5;
    }

    //printf("THREAD CONTROLVOL : SET POWER avant lock\n");
    pthread_mutex_lock(&info->MutexSetPower->mutex);

    //printf("THREAD CONTROLVOL : SET POWER dans lock\n");

    info->high_time=(periode*powerVerified/100.0); // On calcule le nouveaux rapport cyclique.
    info->low_time=periode-info->high_time; //
    pthread_mutex_unlock(&info->MutexSetPower->mutex);

    //printf("THREAD CONTROLVOL : SET POWER apres lock\n");
    return 0;
}


/**
 * Return -1 if FAIL else return 0 in SUCCES
 */
int init_Value_motors(MotorsAll * motorsAll){

    //init 0% de puissance des moteur en fonction de la frequence
    periode=(1.0/frequence)*1000000;

    //init broche du signal du controle des moteur
    int mValues[NUMBER_OF_MOTORS] = {5, 28, 2, 24};

	Motor_info** tab =(Motor_info**) malloc(NUMBER_OF_MOTORS * sizeof(Motor_info*));
	if(tab==NULL){
		logString("MALLOC FAIL : init_Value_motors - Motor_info\n");
		return -1;
	}
	for(int i=0;i<NUMBER_OF_MOTORS;i++){
		tab[i]=(Motor_info *) malloc(sizeof(Motor_info));
		if(tab[i]==NULL){
			logString("MALLOC FAIL : init_Value_motors - Motor_info\n");
			return -1;
		}
	}

	motorsAll->arrayOfMotors=tab;


	for(int i=0;i<NUMBER_OF_MOTORS;i++){
		if(init_Motor_info(motorsAll->arrayOfMotors[i],mValues[i],motorsAll->bool_arret_moteur,motorsAll->Barrier)){
			return -1;
		}
	}
    return 0;

}


/**
 * Return -1 if fail to open threads , else 0
 */
int init_threads_motors(pthread_t * tab,MotorsAll * motorsAll){

	//pthread_t tab[NUMBER_OF_MOTORS];

    pthread_attr_t attributs;

    if(init_Attr_Pthread(&attributs,99,0)){
    	logString("THREAD MAIN : ERROR pthread_attributs MOTORS\n");
    	return -1;
    }

    int error=0;
    int resultPthread=0;

	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		if ((resultPthread = pthread_create(&tab[i], &attributs,thread_startMoteur, motorsAll->arrayOfMotors[i])) != 0) {
			//printf("pthread0_create -> %d\n",resultPthread);
			logString("FAIL pthread_create MOTORS");
			error = -1;
			break;
		}
	}

    if(error==-1){
    	*(motorsAll->bool_arret_moteur)=1;
    }

	pthread_attr_destroy(&attributs);//Libere les resource.

	return error;
}
