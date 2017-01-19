#include "controller.h"

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

	manette->moteur_active = 0;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK); // on initialise les sous-programmes vidéo et joystick

	inputt input; // on crée la structure
	initialiserInput(&input, 0); // on l'initialise au joystick n°0

	int quitter = 0;

	while (!quitter) {

		usleep(Update_Frequence);

		updateEvent(&input); // on récupère les évènements

		if (input.boutons[4] && input.boutons[5] && input.boutons[6]
				&& input.boutons[7]) {
			manette->moteur_active = (manette->moteur_active) ? 0 : 1;
			input.boutons[4] = input.boutons[5] = input.boutons[6] =
					input.boutons[7] = 0;
			printf("bool_moteur_active= %d\n", manette->moteur_active);
			sleep(3);

			pthread_mutex_lock(&argsControl->pmutexControlerPlug->mutex);
			pthread_cond_signal(&argsControl->pmutexControlerPlug->condition);
			pthread_mutex_unlock(&argsControl->pmutexControlerPlug->mutex);
		}

		//printf("APRES IF\n");
		if (manette->moteur_active) {

			pthread_mutex_lock(&argsControl->pmutexReadDataController->mutex);
			argsControl->newThing = 1;

			manette->moteur0 =
					(input.axes[0] < 0) ?
							(float) input.axes[0] * -1 * 5.0 / 32768 + 5 :
							(float) input.axes[0] * 5.0 / 32768 + 5;
			manette->moteur1 =
					(input.axes[1] < 0) ?
							(float) input.axes[1] * -1 * 5.0 / 32768 + 5 :
							(float) input.axes[1] * 5.0 / 32768 + 5;
			manette->moteur2 =
					(input.axes[3] < 0) ?
							(float) input.axes[3] * -1 * 5.0 / 32768 + 5 :
							(float) input.axes[3] * 5.0 / 32768 + 5;
			manette->moteur3 =
					(input.axes[4] < 0) ?
							(float) input.axes[4] * -1 * 5.0 / 32768 + 5 :
							(float) input.axes[4] * 5.0 / 32768 + 5;

			pthread_mutex_unlock(&argsControl->pmutexReadDataController->mutex);

			printf("Axes 1: %f,%f     -       Axes 2: %f,%f \n ",
					manette->moteur0, manette->moteur1, manette->moteur2,
					manette->moteur3);
		}


		if (!is_connect()) {
			manette->moteur_active = 0;
			printf("Plus de manette %d.\n", manette->moteur_active);
			break;
		}
	}

	/* diverses destruction ...*/

	detruireInput(&input); // on détruit la structure input
	SDL_Quit(); // on quitte la SDL

	return;
}
