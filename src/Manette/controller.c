#include "controller.h"

int init_args_CONTROLER(args_CONTROLER ** argControler){

	PMutex * pmutexControllerPlug =(PMutex *) malloc(sizeof(PMutex));
	if (pmutexControllerPlug == NULL) {
		logString("MALLOC FAIL : pmutexControllerPlug");
		return EXIT_FAILURE;
	}
	init_PMutex(pmutexControllerPlug);


	PMutex * pmutexRead =(PMutex *) malloc(sizeof(PMutex));
	if (pmutexRead == NULL) {
		logString("MALLOC FAIL : pmutexRead");
		return EXIT_FAILURE;
	}
	init_PMutex(pmutexRead);

	*argControler =(args_CONTROLER *) malloc(sizeof(args_CONTROLER));
	if (*argControler == NULL) {
		logString("MALLOC FAIL : argControler");
		return EXIT_FAILURE;
	}
	(*argControler)->newThing=0;
	(*argControler)->endController=0;
	(*argControler)->manette=(DataController *) malloc(sizeof( DataController));
	if ((*argControler)->manette == NULL) {
		logString("MALLOC FAIL : argControler->manette");
		return EXIT_FAILURE;
	}
	(*argControler)->pmutexReadDataController=pmutexRead;
	(*argControler)->pmutexControllerPlug=pmutexControllerPlug;

	return 0;
}


void clean_args_CONTROLER(args_CONTROLER * arg) {
	if (arg != NULL) {
		clean_PMutex(arg->pmutexControllerPlug);
		clean_PMutex(arg->pmutexReadDataController);
		clean_DataController(arg->manette);
		free(arg);
		arg = NULL;
	}
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

float pourcent(int valeur, float reference) {
	return ((float) valeur) * 100 / ((float) reference);
}

float diff_axes(int axe_down, int axe_up, int val_max) {
	int val_down = axe_down + val_max;
	int val_up = axe_up + val_max;
	int val_general = val_up - val_down;
	return pourcent(val_general, val_max * 2);
}

void control(args_CONTROLER * argsControl) {
	DataController * manette = argsControl->manette;


	manette->flag = 0;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK); // on initialise les sous-programmes vidéo et joystick

	/*
	 if(verbose){ // code from http://askubuntu.com/questions/366994/how-to-identify-game-controller-for-sdl2-in-ubuntu
	 int num_joysticks = SDL_NumJoysticks();
	 int i;
	 for(i = 0; i < num_joysticks; ++i)
	 {
	 SDL_Joystick* js = SDL_JoystickOpen(i);
	 if (js)
	 {
	 SDL_JoystickGUID guid = SDL_JoystickGetGUID(js);
	 char guid_str[1024];
	 SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
	 const char* name = SDL_JoystickName(js);

	 int num_axes = SDL_JoystickNumAxes(js);
	 int num_buttons = SDL_JoystickNumButtons(js);
	 int num_hats = SDL_JoystickNumHats(js);
	 int num_balls = SDL_JoystickNumBalls(js);

	 printf("%s \"%s\" axes:%d buttons:%d hats:%d balls:%d\n",
	 guid_str, name,
	 num_axes, num_buttons, num_hats, num_balls);

	 SDL_JoystickClose(js);
	 }
	 }
	 }*/

	inputt input; // on crée la structure
	initialiserInput(&input, 0); // on l'initialise au joystick n°0

	int modele;
	int val_max = 32768;

	int quitter = (SDL_NumJoysticks() > 0) ? 0 : 1;

	if ((SDL_NumJoysticks() <= 0)){
		logString("THREAD XBOX : pas de controller");
	}
	float tmpM0, tmpM1, tmpM2, tmpM3;
	while (!quitter) {

		if(argsControl->endController==1){
			quitter=1;
			break;
		}

		usleep(Update_Frequence);

		updateEvent(&input); // on récupère les évènements

		if (input.boutons[4] && input.boutons[5]
				&& ((input.boutons[6] && input.boutons[7])
						|| (input.boutons[9] && input.boutons[10]))) {

			modele = !(input.boutons[9] && input.boutons[10]);
			//modele = 1 pour xbox controller, 0 for ps4 controller

			//TODO choix combinaison demarrage et arret

			if (manette->flag == 2) {
				manette->flag = 0;
			} else {
				manette->flag = 2;
			}

			input.boutons[4] = input.boutons[5] = input.boutons[6] =
					input.boutons[7] = input.boutons[9] = input.boutons[10] = 0;


			char array[400];
			sprintf(array,"THREAD XBOX : bool_moteur_active= %d", manette->flag);
			logString(array);


			while (!modele
					&& (input.axes[4] != -val_max || input.axes[5] != -val_max)) {
				updateEvent(&input);
			}
			sleep(1);

			pthread_mutex_lock(&argsControl->pmutexControllerPlug->mutex);
			pthread_cond_signal(&argsControl->pmutexControllerPlug->condition);
			pthread_mutex_unlock(&argsControl->pmutexControllerPlug->mutex);
		}

		//printf("APRES IF\n");
		if (manette->flag == 2) {
			/*
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
			 */

			//tmpM0 = Rotation axe lacet (Rotation) (y)
			tmpM0 = modele ?
					pourcent(input.axes[0], val_max) :
					pourcent(input.axes[2], val_max);

			//tmpM1 = monter ou descendre (UpDown)
			tmpM1 = modele ?
					pourcent(-1 * input.axes[1], val_max) :
					(diff_axes(input.axes[5], input.axes[4], val_max));

			//tmpM2 = rotation axe roulis (LeftRight) (z)
			tmpM2 = modele ?
					pourcent(input.axes[3], val_max) :
					pourcent(input.axes[0], val_max);

			//tmpM2 = rotation axe tangage (FrontBack) (x)
			tmpM3 = modele ?
					pourcent(-1 * input.axes[4], val_max) :
					pourcent(-1 * input.axes[1], val_max);

			pthread_mutex_lock(&argsControl->pmutexReadDataController->mutex);

			argsControl->newThing = 1;
			manette->axe_Rotation = tmpM0;
			manette->axe_UpDown = tmpM1;
			manette->axe_LeftRight = tmpM2;
			manette->axe_FrontBack = tmpM3;

			pthread_cond_signal(&argsControl->pmutexReadDataController->condition);
			pthread_mutex_unlock(&argsControl->pmutexReadDataController->mutex);

			/*
				printf(
						"Axe_rotation: %f, axe_Up_Down: %f     -       Axes_LeftRight: %f, axe_FrontBack: %f \n ",
						manette->axe_Rotation, manette->axe_UpDown,
						manette->axe_LeftRight, manette->axe_FrontBack);
			*/

		}

		if (!is_connect()) {
			manette->flag = 0;
			char array[400];
			sprintf(array,"THREAD XBOX : NO MORE REMOTTE %d", manette->flag);
			logString(array);

			//TODO
			break;
		}

	}

	/* diverses destruction ...*/
	detruireInput(&input); // on détruit la structure input
	SDL_Quit(); // on quitte la SDL
}
