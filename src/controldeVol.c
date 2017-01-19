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
	float power0;
	float power1;
	float power2;
	float power3;

	//int bool_stop=controle_vol->dataController->moteur_active;


	printf("THREAD CONTROLVOL START\n");
	while(1){

		//printf("THREAD CONTROLVOL avant lock\n");
		//if(controle_vol->motorsAll->bool_arret_moteur){

			pthread_mutex_lock(&(mutexDataControler->mutex));

			//printf("THREAD CONTROLVOL dans lock\n");
			if(mutexDataControler->var<1){
				printf("Controleur de vol attends des nouvel données de serv \n");

				//TODO mettre un timer au wait
				pthread_cond_wait(&mutexDataControler->condition, &mutexDataControler->mutex);
			}
			mutexDataControler->var=0;
			power0=data->moteur0;
			power1=data->moteur1;
			power2=data->moteur2;
			power3=data->moteur3;
			pthread_mutex_unlock(&(mutexDataControler->mutex));
			//printf("THREAD CONTROLVOL apres lock\n");

			//printf("THREAD CONTROLVOL avant set power\n");
			set_power(controle_vol->motorsAll->motor0,power0);
			set_power(controle_vol->motorsAll->motor1,power1);
			set_power(controle_vol->motorsAll->motor2,power2);
			set_power(controle_vol->motorsAll->motor3,power3);

		/*
		}
		else{
			perror("Arret des moteurs demandé: Procedur Non defini.\n");
		}
		*/

	}
}
