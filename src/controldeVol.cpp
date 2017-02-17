#include "controldeVol.hpp"


int init_args_CONTROLDEVOL(args_CONTROLDEVOL ** argCONTROLVOL,DataController * dataControl,MotorsAll * motorsAll){

	*argCONTROLVOL =(args_CONTROLDEVOL *) malloc(sizeof(args_CONTROLDEVOL));
	if (*argCONTROLVOL == NULL) {
		logString("MALLOC FAIL : init_args_CONTROLDEVOL");
		return EXIT_FAILURE;
	}

	(*argCONTROLVOL)->dataController = dataControl;
	(*argCONTROLVOL)->motorsAll = motorsAll;

	return 0;
}

void clean_args_CONTROLDEVOL(args_CONTROLDEVOL * arg) {
	if (arg != NULL) {
		clean_MotorsAll(arg->motorsAll);
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

	logString("THREAD CONTROLVOL : START");


	//test();
	//calibrate(args);
	args_CONTROLDEVOL  * controle_vol =(args_CONTROLDEVOL  *)args;
	DataController * data = controle_vol->dataController;
	PMutex * mutexDataControler =controle_vol->dataController->pmutex;
	RTIMU *imu =(RTIMU *)controle_vol->imu;


	float powerTab[NUMBER_OF_MOTORS];



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

	RTIMU_DATA imuData;

	int continuThread=1;
	while (continuThread) {

		sleep(5);
		#ifdef __arm__
		if(imu->IMURead()){
			RTIMU_DATA imuData = imu->getIMUData();
			sampleCount++;
			now = RTMath::currentUSecsSinceEpoch();

			//printf("*******************************\nTHREAD CONTROLVOL : CAPTEUR -> %s\n*******************************\n", RTMath::displayDegrees("", imuData.fusionPose));
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
			logString("Controleur de vol attends des nouvel données de serv \n");

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
		for(int i=0; i<NUMBER_OF_MOTORS;i++){
			set_power(controle_vol->motorsAll->arrayOfMotors[i], powerTab[i]);
		}

	}
	logString("THREAD CONTROLVOL : END");
	return NULL;
}


/**
 * return 0 if Succes
 */
int init_thread_PID(pthread_t * threadControlerVOL,args_CONTROLDEVOL * argCONTROLVOL){

	pthread_attr_t attributs;
	if(init_Attr_Pthread(&attributs,99,1)){
		logString("THREAD MAIN : ERROR pthread_attributs PID");
		return -1;
	}

	int result=pthread_create(threadControlerVOL, &attributs, startCONTROLVOL, argCONTROLVOL);
	if (result) {
		logString("THREAD MAIN : ERROR pthread_create PID");
	}

	pthread_attr_destroy(&attributs);

	return result;
}
