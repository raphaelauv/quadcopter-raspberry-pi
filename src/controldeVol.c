#include "controldeVol.h"

void * startCONTROLVOL(void * args){

	args_CONTROLDEVOL  * controle_vol =args;
	dataController * data = controle_vol->dataController;
	float power0;
	float power1;
	float power2;
	float power3;
	while(1){

		if(controle_vol->motorsAll->bool_arret_moteur){

			pthread_mutex_lock(&controle_vol->mutexDataControler->mutex);
			if(controle_vol->mutexDataControler->var<1){
				printf("Controleur de vol attends des nouvel données de serv \n");

				//TODO mettre un timer au wait

				pthread_cond_wait(&controle_vol->mutexDataControler->condition, &controle_vol->mutexDataControler->mutex);
			}
			controle_vol->mutexDataControler->var=0;

			power0=data->moteur0;
			power1=data->moteur1;
			power2=data->moteur2;
			power3=data->moteur3;
			pthread_mutex_lock(&controle_vol->mutexDataControler->mutex);

			set_power(controle_vol->motorsAll->motor0,power0);
			set_power(controle_vol->motorsAll->motor1,power1);
			set_power(controle_vol->motorsAll->motor2,power2);
			set_power(controle_vol->motorsAll->motor3,power3);

		}
		else{
			perror("Arret des moteurs demandé: Procedur Non defini.\n");
		}

	}
}
