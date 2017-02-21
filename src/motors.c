#include "motors.h"
#include "concurrent.h"

//double frequence=50.0; //frequence du signal d'entré des ESCs
double periode=0; // periode = 1/frequence. Initialisée plus tard.


/**********************************************************************/
/* MULTI THREADING SOLUTION */

int init_MotorsAll(MotorsAll ** motorsAll){

	#ifdef __arm__
	if (wiringPiSetup () == -1){
		return -1;
	}
	#endif

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

void * thread_startMotor(void * args){

	Motor_info * info=(Motor_info *)args;

    int low,hight;

	#ifdef __arm__
    //if (wiringPiSetup () == -1){
    //    return NULL;
    //}
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
        	pthread_mutex_unlock(&info->MutexSetPower->mutex);
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

int set_power(Motor_info * info,int high_time){

    int low=20000-high_time;

    pthread_mutex_lock(&info->MutexSetPower->mutex);

    info->high_time=high_time; // On calcule le nouveaux rapport cyclique.
    info->low_time=low;
    pthread_mutex_unlock(&info->MutexSetPower->mutex);

}


/**
 * Return -1 if FAIL else return 0 in SUCCES
 */
int init_Value_motors(MotorsAll * motorsAll){

    //init 0% de puissance des moteur en fonction de la frequence
    periode=(1.0/FREQUENCY)*1000000;

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
    	logString("THREAD MAIN : ERROR pthread_attributs MOTORS");
    	return -1;
    }

    int error=0;
    int resultPthread=0;

	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		if ((resultPthread = pthread_create(&tab[i], &attributs,thread_startMotor, motorsAll->arrayOfMotors[i])) != 0) {
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



/**********************************************************************/
/* ONE THREAD SOLUTION */


int init_MotorsAll2(MotorsAll2 ** motorsAll2,int NbMotors,...){
	#ifdef __arm__
	if (wiringPiSetup () == -1){
		return -1;
	}
	#endif

	if(NbMotors!=NUMBER_OF_MOTORS){
		logString("init_MotorsAll2 FAIL : NbMotors arg different of MACRO NUMBER_OF_MOTORS");
		return -1;
	}

	*motorsAll2 =(MotorsAll2 *) malloc(sizeof(MotorsAll2));
	if (*motorsAll2 == NULL) {
		logString("MALLOC FAIL : motorsAll");
		return -1;
	}
	(*motorsAll2)->bool_arret_moteur =(volatile int *) malloc(sizeof(int));

	if ((*motorsAll2)->bool_arret_moteur == NULL) {
		logString("MALLOC FAIL : motorsAll->bool_arret_moteur");
		return -1;
	}
	volatile int arret=0;//TODO
	(*(*motorsAll2)->bool_arret_moteur)= arret;


	PMutex * barrier = (PMutex *) malloc(sizeof(PMutex));
	if (barrier == NULL) {
		logString("MALLOC FAIL : barrier");
		return -1;
	}
	init_PMutex(barrier);

	(*motorsAll2)->MutexSetValues=barrier;

	va_list va;
	va_start (va, NbMotors);

	char array[400];
	sprintf(array, "NUMBER OF MOTORS :%d",NUMBER_OF_MOTORS);
	logString(array);
	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		int c = va_arg(va, int);
		(*motorsAll2)->broche[i]=c;
		sprintf(array, "BROCHE %d VALUE : %d",i,(*motorsAll2)->broche[i]);
		logString(array);
	}

	return 0;

}

void clean_MotorsAll2(MotorsAll2 * arg) {
	if (arg != NULL) {
		if (arg->MutexSetValues != NULL) {
			free(arg->MutexSetValues);
			arg->MutexSetValues = NULL;
		}

		if (arg->bool_arret_moteur != NULL) {
			free((void *)arg->bool_arret_moteur);
			arg->bool_arret_moteur=NULL;
		}
		free(arg);
		arg = NULL;
	}
}


int printArray2D(int array[NUMBER_OF_MOTORS][2]){
	printf("-----------\n");
	for(int i=0;i<NUMBER_OF_MOTORS;i++){
		printf("BROCHE : %d -> val : %d\n",array[i][0],array[i][1]);
	}
	printf("-----------\n");
}

int comp (const void * elem1, const void * elem2)
{
    int f = ((int*)elem1)[1];
    int s = ((int*)elem2)[1];
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}

void * thread_startMotorAll(void * args){
	MotorsAll2 * motors =(MotorsAll2*) args;

	int period=(1.0/FREQUENCY)*1000000;

	int valuesBrocheMotor[NUMBER_OF_MOTORS][2];

	#ifdef __arm__
	for(int i=0;i<NUMBER_OF_MOTORS;i++){
		pinMode (motors->broche[i], OUTPUT);
	}
	#endif

	logString("THREAD MOTORS : INIT DONE");

	int runMotor=1;

	srand(time(NULL));
	while (runMotor) {
		sleep(5);
		for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
			valuesBrocheMotor[i][0] = motors->broche[i];
		}

		pthread_mutex_lock(&motors->MutexSetValues->mutex);
		if ((*(motors->bool_arret_moteur)) != 1) {
			for(int i =0;i<NUMBER_OF_MOTORS;i++){

				int ale=(int) (((double)(15+1)/RAND_MAX) * rand() + 0);
				//valuesBrocheMotor[i][1]=motors->high_time[i];
				valuesBrocheMotor[i][1]=ale;
			}

			pthread_mutex_unlock(&motors->MutexSetValues->mutex);

			qsort(valuesBrocheMotor, NUMBER_OF_MOTORS, sizeof valuesBrocheMotor[0], comp);

			//printArray2D(valuesBrocheMotor);

			for(int i=0;i<NUMBER_OF_MOTORS;i++){
				#ifdef __arm__
				//digitalWrite(valuesBrocheMotor[i][0],1);
				#endif
			}

			int sleepedTotalTime=0;
			int dif=0;
			for (int i = 0; i < NUMBER_OF_MOTORS; i++) {

				dif=valuesBrocheMotor[i][1]-sleepedTotalTime;
				printf("SLEEP %d : %d\n",i,dif);
				usleep(dif);
				sleepedTotalTime+=dif;
				#ifdef __arm__
				//digitalWrite(valuesBrocheMotor[i][0],0);
				#endif
			}
			printf("SLEEP FINAL : %d\n",20-sleepedTotalTime);
			usleep(period-sleepedTotalTime);

		} else {
			pthread_mutex_unlock(&motors->MutexSetValues->mutex);
			runMotor = 0;
		}

	}
	logString("THREAD MOTORS : END");
	return NULL;
}


/**
 * Return -1 if FAIL to open the thread , else 0 in SUCCES
 */
int init_thread_startMotorAll2(pthread_t * pthread,MotorsAll2 * motorsAll2){

    pthread_attr_t attributs;
    int error=0;

    if(init_Attr_Pthread(&attributs,99,0)){
    	logString("THREAD MAIN : ERROR pthread_attributs MOTORS");
    	return -1;
    }

	if (pthread_create(pthread, &attributs,thread_startMotorAll,motorsAll2)) {
		error=-1;
		logString("FAIL pthread_create MOTORS");
	}

    if(error==-1){
    	*(motorsAll2->bool_arret_moteur)=1;
    }

    pthread_attr_destroy(&attributs);

	return error;
}

int set_power2(MotorsAll2 * MotorsAll2, float * powers){

	pthread_mutex_lock(&MotorsAll2->MutexSetValues->mutex);

	for(int i =0;i<NUMBER_OF_MOTORS;i++){
		MotorsAll2->high_time[i]=powers[i];
	}
	pthread_mutex_unlock(&MotorsAll2->MutexSetValues->mutex);
}
