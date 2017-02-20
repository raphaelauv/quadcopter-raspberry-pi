#include "PID.hpp"


int init_args_PID(args_PID ** argPID,DataController * dataControl,MotorsAll * motorsAll){

	*argPID =(args_PID *) malloc(sizeof(args_PID));
	if (*argPID == NULL) {
		logString("MALLOC FAIL : init_args_PID");
		return EXIT_FAILURE;
	}

	(*argPID)->dataController = dataControl;
	(*argPID)->motorsAll = motorsAll;

	return 0;
}

void clean_args_PID(args_PID * arg) {
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


void * thread_PID(void * args);
/**
 * return 0 if Succes
 */
int init_thread_PID(pthread_t * threadPID,args_PID * argPID){

	pthread_attr_t attributs;
	if(init_Attr_Pthread(&attributs,99,1)){
		logString("THREAD MAIN : ERROR pthread_attributs PID");
		return -1;
	}

	int result=pthread_create(threadPID, &attributs, thread_PID, argPID);
	if (result) {
		logString("THREAD MAIN : ERROR pthread_create PID");
	}

	pthread_attr_destroy(&attributs);

	return result;
}


void * thread_PID(void * args){

	logString("THREAD PID : START");
	//test();
	//calibrate(args);
	args_PID  * controle_vol =(args_PID  *)args;
	DataController * data = controle_vol->dataController;
	PMutex * mutexDataControler =controle_vol->dataController->pmutex;
	RTIMU *imu =(RTIMU *)controle_vol->imu;


	float powerTab[NUMBER_OF_MOTORS];

	uint64_t time_debut;
	uint64_t time_fin;
	uint64_t time_to_sleep = 0;

	//Consigne client
	float client_gaz = 1050;
	float client_pitch = 0;

	//PID
	float output_pid_pitch = 0;
	//erreur des PID
	float pid_erreur_tmp_pitch = 0;

	//erreur accu
	float pid_accu_erreur_pitch = 0;

	//last erreur
	float pid_last_pitch = 0;

	// puissance des 4 moteur. (en microseconde)
	int puissance_motor0 = 1000;
	int puissance_motor1 = 1000;


	RTIMU_DATA imuData;

	int continuThread=1;

	while (continuThread) {


		#ifdef __arm__
		time_debut=RTMath::currentUSecsSinceEpoch();


		while(imu->IMURead()){
			imuData = imu->getIMUData();

			//calcule pitch PID
			pid_erreur_tmp_pitch=(imuData.fusionPose.y()*RTMATH_RAD_TO_DEGREE)-client_pitch;
			pid_accu_erreur_pitch+=PID_GAIN_I_PITCH*pid_erreur_tmp_pitch;
			if (pid_accu_erreur_pitch>PID_MAX_PITCH) {
				pid_accu_erreur_pitch=PID_MAX_PITCH;
			}
			else if (pid_accu_erreur_pitch < -PID_MAX_PITCH){
				pid_accu_erreur_pitch=-PID_MAX_PITCH;
			}

			output_pid_pitch=PID_GAIN_P_PITCH*pid_erreur_tmp_pitch+pid_accu_erreur_pitch+PID_GAIN_D_PITCH*(pid_erreur_tmp_pitch-pid_last_pitch);

			if (output_pid_pitch>PID_MAX_PITCH) {
				output_pid_pitch=PID_MAX_PITCH;
			}
			else if (output_pid_pitch < -PID_MAX_PITCH){
				output_pid_pitch=-PID_MAX_PITCH;
			}
			pid_last_pitch=pid_erreur_tmp_pitch;


			puissance_motor0=client_gaz + output_pid_pitch;
			puissance_motor1=client_gaz - output_pid_pitch;

			//Pour jamais mettre a l'arret les moteur.
			if(puissance_motor0<1100) puissance_motor0=1100;
			if(puissance_motor1<1100) puissance_motor1=1100;

			//puissance max=2000 donc il faut pas depasser.
			if(puissance_motor0>2000) puissance_motor0=2000;
			if(puissance_motor1>2000) puissance_motor1=2000;


			//set la puissance au moteur.
			set_power(controle_vol->motorsAll->arrayOfMotors[0], puissance_motor0);
			set_power(controle_vol->motorsAll->arrayOfMotors[1], puissance_motor1);

			break;
		}

		#endif

		/*********************************************************/
		/*					CODE FOR GET REMOTE					*/


		pthread_mutex_lock(&(mutexDataControler->mutex));

		if (data->flag== 0) {
			pthread_mutex_unlock(&(mutexDataControler->mutex));

			*(controle_vol->motorsAll->bool_arret_moteur)=1;

			continuThread = 0;
			continue;
		}

		mutexDataControler->var = 0;
		powerTab[0] = data->axe_Rotation;
		powerTab[1] = data->axe_UpDown;
		powerTab[2] = data->axe_LeftRight;
		powerTab[3] = data->axe_FrontBack;

		pthread_mutex_unlock(&(mutexDataControler->mutex));

		/*
		for(int i=0; i<NUMBER_OF_MOTORS;i++){
			set_power(controle_vol->motorsAll->arrayOfMotors[i], powerTab[i]);
		}

		*/

		/*********************************************************/
		/*			CODE FOR SLEEP PID FREQUENCY				*/

		#ifdef __arm__
		time_fin=RTMath::currentUSecsSinceEpoch();
		time_to_sleep=(time_fin -time_debut);
		if(time_fin-time_debut>4000){
			printf("temps sleep : %lld\n",4000-time_to_sleep);
		}
		usleep(4000-time_to_sleep);
		#endif


	}
	logString("THREAD PID : END");
	return NULL;
}
