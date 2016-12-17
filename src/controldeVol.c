#include "controldeVol.h"

void * startCONTROLVOL(void * args){

	args_CONTROLDEVOL  * controle_vol =args;
	while(1){
		dataController * data = controle_vol->dataController;
		if(!controle_vol->motorsAll->bool_arret_moteur){
			set_power(controle_vol->motorsAll->motor0,data->moteur0);
			set_power(controle_vol->motorsAll->motor1,data->moteur1);
			set_power(controle_vol->motorsAll->motor2,data->moteur2);
			set_power(controle_vol->motorsAll->motor3,data->moteur3);
		}
		else{
			perror("Arret des moteurs demandé: Procedur Non defini.\n");
		}
		sleep(1);
	}


}
