#include "controldeVol.h"

void clean_args_CONTROLDEVOL(args_CONTROLDEVOL * arg) {
	if (arg != NULL) {
		clean_motorsAll(arg->motorsAll);
		clean_DataController(arg->dataController);
	}
	free(arg);
	arg = NULL;
}


void * startCONTROLVOL(void * args){

	args_CONTROLDEVOL  * controle_vol =(args_CONTROLDEVOL  *)args;
	DataController * data = controle_vol->dataController;
	PMutex * mutexDataControler =controle_vol->dataController->pmutex;

	char verbose = controle_vol->verbose;
	float power0;
	float power1;
	float power2;
	float power3;

	if(verbose){printf("THREAD CONTROLVOL : START\n");}
	int continuThread=1;
	while (continuThread) {

		pthread_mutex_lock(&(mutexDataControler->mutex));

		if (data->moteur_active == 0) {
			pthread_mutex_unlock(&(mutexDataControler->mutex));

			*controle_vol->motorsAll->bool_arret_moteur=1;

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
		power0 = data->moteur0;
		power1 = data->moteur1;
		power2 = data->moteur2;
		power3 = data->moteur3;
		pthread_mutex_unlock(&(mutexDataControler->mutex));
		set_power(controle_vol->motorsAll->motor0, power0);
		set_power(controle_vol->motorsAll->motor1, power1);
		set_power(controle_vol->motorsAll->motor2, power2);
		set_power(controle_vol->motorsAll->motor3, power3);
	}
	if(verbose){printf("THREAD CONTROLVOL : END\n");}
	return NULL;
}
