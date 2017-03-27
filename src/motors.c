#include "motors.h"

int init_MotorsAll2(MotorsAll2 ** motorsAll2){
	#ifdef __arm__
	if (wiringPiSetup () == -1){
		return -1;
	}
	#endif

/*
	if(NbMotors!=NUMBER_OF_MOTORS){
		logString("init_MotorsAll2 FAIL : NbMotors arg different of MACRO NUMBER_OF_MOTORS");
		return -1;
	}
*/
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

	(*(*motorsAll2)->bool_arret_moteur)= 0;


	PMutex * barrier = (PMutex *) malloc(sizeof(PMutex));
	if (barrier == NULL) {
		logString("MALLOC FAIL : barrier");
		return -1;
	}
	init_PMutex(barrier);

	(*motorsAll2)->MutexSetValues=barrier;

	int arrayValuesBrocheMotor [8]={BROCHE_MOTOR_0 ,BROCHE_MOTOR_1,BROCHE_MOTOR_2,BROCHE_MOTOR_3,BROCHE_MOTOR_4,BROCHE_MOTOR_5,BROCHE_MOTOR_6,BROCHE_MOTOR_7};

	char array[SIZE_MAX_LOG];
	sprintf(array, "NUMBER OF MOTORS :%d",NUMBER_OF_MOTORS);
	logString(array);
	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		
		(*motorsAll2)->broche[i]=arrayValuesBrocheMotor[i];
		(*motorsAll2)->high_time[i]=MOTOR_LOW_TIME;
		sprintf(array, "BROCHE %d VALUE : %d",i,(*motorsAll2)->broche[i]);
		logString(array);
	}

	return 0;

}

void clean_MotorsAll2(MotorsAll2 * arg) {
	if (arg != NULL) {

		clean_PMutex(arg->MutexSetValues);

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

	int local_period=(1.0/FREQUENCY_MOTOR)*USEC_TO_SEC;

	int valuesBrocheMotor[NUMBER_OF_MOTORS][2];

	#ifdef __arm__
	for(int i=0;i<NUMBER_OF_MOTORS;i++){
		pinMode (motors->broche[i], OUTPUT);
	}
	#endif

	logString("THREAD MOTORS : INIT DONE");

	int runMotor=1;
	//srand(time(NULL));
	int timeUsecStart=0;
	int timeUsecEnd=0;
	int timeBetween=0;
	struct timeval tv;
	
	int i;
	int sleepedTotalTime=0;
	int dif=0;
	char arrayLog[SIZE_MAX_LOG];

	logString("THREAD MOTORS : START");

	int tmp=0;

	while (runMotor) {


		gettimeofday(&tv, NULL);
		timeUsecStart= (int)tv.tv_sec * USEC_TO_SEC + (int)tv.tv_usec;

		for (i = 0; i < NUMBER_OF_MOTORS; i++) {
			valuesBrocheMotor[i][0] = motors->broche[i];
		}

		pthread_mutex_lock(&motors->MutexSetValues->mutex);
		
		if ((*(motors->bool_arret_moteur)) != 1) {
			
			for(i =0;i<NUMBER_OF_MOTORS;i++){

				valuesBrocheMotor[i][1]=motors->high_time[i];
				//int ale=(int) (((double)(15+1)/RAND_MAX) * rand() + 0);
				//valuesBrocheMotor[i][1]=ale;
			}

			pthread_mutex_unlock(&motors->MutexSetValues->mutex);

			qsort(valuesBrocheMotor, NUMBER_OF_MOTORS, sizeof valuesBrocheMotor[0], comp);

			//printArray2D(valuesBrocheMotor);

			for(i=0;i<NUMBER_OF_MOTORS;i++){
				#ifdef __arm__
				digitalWrite(valuesBrocheMotor[i][0],1);
				#endif
			}

			sleepedTotalTime=0;
			dif=0;
			for (i = 0; i < NUMBER_OF_MOTORS; i++) {

				dif=(valuesBrocheMotor[i][1])-sleepedTotalTime;
				//printf("SLEEP %d : %d\n",i,dif);
				if(dif>0){
					if(sleepedTotalTime+dif<= MOTOR_HIGH_TIME){
						//usleep(dif);
						nanoSleepSecure(dif * NSEC_TO_USEC_MULTIPLE);
						sleepedTotalTime+=dif;
					}else{

						tmp=MOTOR_HIGH_TIME-sleepedTotalTime;
						if(tmp>0){
							nanoSleepSecure((tmp)* NSEC_TO_USEC_MULTIPLE);
						}
						sleepedTotalTime=MOTOR_HIGH_TIME;
					}
				}
				#ifdef __arm__
				digitalWrite(valuesBrocheMotor[i][0],0);
				#endif
			}
			//printf("SLEEP FINAL : %d\n",period-sleepedTotalTime);

			gettimeofday(&tv, NULL);
			timeUsecEnd = (int)tv.tv_sec * USEC_TO_SEC + (int)tv.tv_usec;
			timeBetween = timeUsecEnd - timeUsecStart ;
			if(timeBetween > local_period){
				logString("THREAD MOTORS : ERROR PERIODE TOO SLOW");
			}else{
				if(timeBetween > MOTOR_HIGH_TIME +100 ){
					sprintf(arrayLog,"THREAD MOTOR : TIME : %d\n",timeBetween);
					logString(arrayLog);
				}

				//usleep(local_period - timeBetween);
				nanoSleepSecure((local_period - timeBetween)* NSEC_TO_USEC_MULTIPLE);
			}

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
int init_thread_startMotorAll2(pthread_t * pthread,void * threadMotor2_stack_buf,MotorsAll2 * motorsAll2){

    pthread_attr_t attributs;
    int error=0;

    if(init_Attr_Pthread(&attributs,CPU_PRIORITY_MOTOR,CPU_CORE_MOTOR,threadMotor2_stack_buf)){
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

int set_power2(MotorsAll2 * MotorsAll2, int * powers){

	pthread_mutex_lock(&MotorsAll2->MutexSetValues->mutex);

	for(int i =0;i<NUMBER_OF_MOTORS;i++){
		MotorsAll2->high_time[i]=powers[i];
	}
	pthread_mutex_unlock(&MotorsAll2->MutexSetValues->mutex);
}

void setMotorStop(MotorsAll2 * MotorsAll2){

	pthread_mutex_lock(&MotorsAll2->MutexSetValues->mutex);

	*(MotorsAll2->bool_arret_moteur)=1;

	pthread_mutex_unlock(&MotorsAll2->MutexSetValues->mutex);
}

int isMotorStop(MotorsAll2 * MotorsAll2){

	int value;
	pthread_mutex_lock(&MotorsAll2->MutexSetValues->mutex);

	value=*(MotorsAll2->bool_arret_moteur);

	pthread_mutex_unlock(&MotorsAll2->MutexSetValues->mutex);
	return value;
}
