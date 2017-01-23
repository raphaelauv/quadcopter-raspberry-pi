#include "controller.h"

void clean_args_CONTROLER(args_CONTROLER * arg) {
	if (arg != NULL) {
		clean_PMutex(arg->pmutexControlerPlug);
		clean_PMutex(arg->pmutexReadDataController);
		clean_DataController(arg->manette);
	}
	free(arg);
	arg = NULL;
}

char is_connect() {
	//char name[100];
	//strcpy(name, "Microsoft X-Box 360 pad");

	const char * tmp = SDL_JoystickName(0);
	if (tmp != NULL) {
		return 1;
	}
	return 0;
}

void control(args_CONTROLER * argsControl) {

	DataController * manette = argsControl->manette;
	char verbose = argsControl->verbose;

	manette->moteur_active = 0;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK); // on initialise les sous-programmes vidéo et joystick

	inputt input; // on crée la structure
	initialiserInput(&input, 0); // on l'initialise au joystick n°0

	int quitter = 0;

	float tmpM0,tmpM1,tmpM2,tmpM3;

	while (!quitter) {

		usleep(Update_Frequence);

		updateEvent(&input); // on récupère les évènements

		if (input.boutons[4] && input.boutons[5] && input.boutons[6]
				&& input.boutons[7]) {
			manette->moteur_active = (manette->moteur_active) ? 0 : 1;
			/*TODO
			  <=> manette->moteur_active = !(manette->moteur_active)
			*/
			input.boutons[4] = input.boutons[5] = input.boutons[6] =
					input.boutons[7] = 0;
			if(verbose){printf("bool_moteur_active= %d\n", manette->moteur_active);}
			sleep(3);

			pthread_mutex_lock(&argsControl->pmutexControlerPlug->mutex);
			pthread_cond_signal(&argsControl->pmutexControlerPlug->condition);
			pthread_mutex_unlock(&argsControl->pmutexControlerPlug->mutex);
		}

		//printf("APRES IF\n");
		if (manette->moteur_active) {
		  
			tmpM0 = (input.axes[0] < 0) ?
					(float) input.axes[0] * -1 * 5.0 / 32768 + 5 :
					(float) input.axes[0] * 5.0 / 32768 + 5;
			tmpM1 = (input.axes[1] < 0) ?
					(float) input.axes[1] * -1 * 5.0 / 32768 + 5 :
					(float) input.axes[1] * 5.0 / 32768 + 5;
			tmpM2 = (input.axes[3] < 0) ?
					(float) input.axes[3] * -1 * 5.0 / 32768 + 5 :
					(float) input.axes[3] * 5.0 / 32768 + 5;
			tmpM3 = (input.axes[4] < 0) ?
					(float) input.axes[4] * -1 * 5.0 / 32768 + 5 :
					(float) input.axes[4] * 5.0 / 32768 + 5;
		  

/*
		  tmpM0 = input.axes[0];

		  tmpM1 = input.axes[1];

		  tmpM2 = input.axes[3];

		  tmpM3 = input.axes[4];
*/
		  
			pthread_mutex_lock(&argsControl->pmutexReadDataController->mutex);

			argsControl->newThing = 1;
			manette->axe_Rotation =tmpM0;
			manette->axe_UpDown =tmpM1;
			manette->axe_LeftRight =tmpM2;
			manette->axe_FrontBack =tmpM3;

			pthread_mutex_unlock(&argsControl->pmutexReadDataController->mutex);

			if(verbose){printf("Axes 1: %f,%f     -       Axes 2: %f,%f \n ",
					manette->axe_Rotation, manette->axe_UpDown, manette->axe_LeftRight,
					manette->axe_FrontBack);}

			  sleep(3);
			}


			if (!is_connect()) {
				manette->moteur_active = 0;
				if(verbose){printf("Plus de manette %d.\n", manette->moteur_active);}
				break;
			}

		}



	/* diverses destruction ...*/

	detruireInput(&input); // on détruit la structure input
	SDL_Quit(); // on quitte la SDL

	return;
}
