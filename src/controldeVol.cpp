#include "controldeVol.hpp"


int initArgsCONTROLDEVOL(args_CONTROLDEVOL ** argCONTROLVOL,DataController * dataControl,MotorsAll * motorsAll,char verbose){

	*argCONTROLVOL =(args_CONTROLDEVOL *) malloc(sizeof(args_CONTROLDEVOL));
	if (*argCONTROLVOL == NULL) {
		perror("MALLOC FAIL : argCONTROLVOL\n");
		return EXIT_FAILURE;
	}

	(*argCONTROLVOL)->dataController = dataControl;
	(*argCONTROLVOL)->motorsAll = motorsAll;
	(*argCONTROLVOL)->verbose = verbose;

	return 0;
}

void clean_args_CONTROLDEVOL(args_CONTROLDEVOL * arg) {
	if (arg != NULL) {
		clean_motorsAll(arg->motorsAll);
		clean_DataController(arg->dataController);
		if( arg->imu !=NULL){
			delete(arg->imu);
			arg->imu=NULL;
		}
		free(arg);
		arg = NULL;
	}
}


void * startCONTROLVOL(void * args){

	//test();
	//calibrate(args);
	args_CONTROLDEVOL  * controle_vol =(args_CONTROLDEVOL  *)args;
	DataController * data = controle_vol->dataController;
	PMutex * mutexDataControler =controle_vol->dataController->pmutex;
	RTIMU *imu =(RTIMU *)controle_vol->imu;

	char verbose = controle_vol->verbose;
	float powerTab[4];
	/*
	float power0;
	float power1;
	float power2;
	float power3;
	*/


	int sampleCount = 0;
	int sampleRate = 0;
	uint64_t rateTimer;
	uint64_t displayTimer;
	uint64_t now;



	//  set up for rate timer

	rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();

	//  now just process data
	//double power1=5.0;
	//double power2=5.0;
	double pitch=0.0;



	if(verbose){printf("THREAD CONTROLVOL : START\n");}


	RTIMU_DATA imuData;

	int continuThread=1;
	while (continuThread) {

		sleep(5);
		#ifdef __arm__
		if(imu->IMURead()){
			RTIMU_DATA imuData = imu->getIMUData();
			sampleCount++;
			now = RTMath::currentUSecsSinceEpoch();
			if(verbose){
			//printf("*******************************\nTHREAD CONTROLVOL : CAPTEUR -> %s\n*******************************\n", RTMath::displayDegrees("", imuData.fusionPose));
			}
			//fflush(stdout);
		}
		#endif

		pthread_mutex_lock(&(mutexDataControler->mutex));

		if (data->flag== 0) {
			pthread_mutex_unlock(&(mutexDataControler->mutex));

			*(controle_vol->motorsAll->bool_arret_moteur)=1;

			continuThread = 0;
			continue;
		}

		/*
		if (mutexDataControler->var < 1) {
			//if(verbose){printf("Controleur de vol attends des nouvel données de serv \n");}

			//TODO mettre un timer au wait
			pthread_cond_wait(&mutexDataControler->condition,
					&mutexDataControler->mutex);
		}
		*/
		mutexDataControler->var = 0;
		powerTab[0] = data->axe_Rotation;
		powerTab[1] = data->axe_UpDown;
		powerTab[2] = data->axe_LeftRight;
		powerTab[3] = data->axe_FrontBack;


		pthread_mutex_unlock(&(mutexDataControler->mutex));
		for(int i=0; i<4;i++){
			set_power(controle_vol->motorsAll->arrayOfMotors[i], powerTab[i]);
		}
		/*
		set_power(controle_vol->motorsAll->motor0, power0);
		set_power(controle_vol->motorsAll->motor1, power1);
		set_power(controle_vol->motorsAll->motor2, power2);
		set_power(controle_vol->motorsAll->motor3, power3);
		*/
	}
	if(verbose){printf("THREAD CONTROLVOL : END\n");}
	return NULL;
}


/**
 * return 0 if Succes
 */
int init_thread_PID(pthread_t * threadControlerVOL,args_CONTROLDEVOL * argCONTROLVOL){

	pthread_attr_t attributs;
	if(init_Attr_Pthread(&attributs,99,1)){
		perror("THREAD MAIN : ERROR pthread_attributs PID\n");
		return -1;
	}

	int result;

	if ((result=pthread_create(threadControlerVOL, &attributs, startCONTROLVOL, argCONTROLVOL))!=0) {
			perror("THREAD MAIN : ERROR pthread_create PID\n");
		}

	pthread_attr_destroy(&attributs);

	return result;
}
