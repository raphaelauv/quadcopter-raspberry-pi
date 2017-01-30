#include "controldeVol.hpp"

void clean_args_CONTROLDEVOL(args_CONTROLDEVOL * arg) {
	if (arg != NULL) {
		clean_motorsAll(arg->motorsAll);
		clean_DataController(arg->dataController);
		if( arg->imu !=NULL){
			delete(arg->imu);
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
	float power0;
	float power1;
	float power2;
	float power3;



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

		#ifdef __arm__
		if(imu->IMURead()){
			RTIMU_DATA imuData = imu->getIMUData();
			sampleCount++;
			now = RTMath::currentUSecsSinceEpoch();
			printf("*******************************\nSample rate : %s\n*******************************\n", RTMath::displayDegrees("", imuData.fusionPose));
			fflush(stdout);
		}
		#endif

		pthread_mutex_lock(&(mutexDataControler->mutex));

		if (data->flag== 0) {
			pthread_mutex_unlock(&(mutexDataControler->mutex));

			*(controle_vol->motorsAll->bool_arret_moteur)=1;

			continuThread = 0;
			continue;
		}

		if (mutexDataControler->var < 1) {
			if(verbose){printf("Controleur de vol attends des nouvel données de serv \n");}

			//TODO mettre un timer au wait
			pthread_cond_wait(&mutexDataControler->condition,
					&mutexDataControler->mutex);
		}
		mutexDataControler->var = 0;
		power0 = data->axe_Rotation;
		power1 = data->axe_UpDown;
		power2 = data->axe_LeftRight;
		power3 = data->axe_FrontBack;
		pthread_mutex_unlock(&(mutexDataControler->mutex));
		set_power(controle_vol->motorsAll->motor0, power0);
		set_power(controle_vol->motorsAll->motor1, power1);
		set_power(controle_vol->motorsAll->motor2, power2);
		set_power(controle_vol->motorsAll->motor3, power3);
	}
	if(verbose){printf("THREAD CONTROLVOL : END\n");}
	return NULL;
}
