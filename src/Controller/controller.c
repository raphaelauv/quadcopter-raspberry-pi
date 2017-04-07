#include "controller.h"


void signalController(args_CONTROLLER * argsControl){
	pthread_mutex_lock(&argsControl->pmutexControllerPlug->mutex);
	pthread_cond_signal(&argsControl->pmutexControllerPlug->condition);
	pthread_mutex_unlock(&argsControl->pmutexControllerPlug->mutex);

}

void set_Controller_Stop(args_CONTROLLER * argControler){
	pthread_mutex_lock(&argControler->pmutexReadDataController->mutex);
	argControler->controllerStop=1;
	pthread_mutex_unlock(&argControler->pmutexReadDataController->mutex);
}

int is_Controller_Stop(args_CONTROLLER * argControler){
	//first look to glabal signal value
	int value=*(argControler->boolStopController);
	if(value){
		pthread_mutex_lock(&argControler->pmutexReadDataController->mutex);
		argControler->controllerStop=1;
		pthread_mutex_unlock(&argControler->pmutexReadDataController->mutex);
		return value;

	//or look to the atomic value
	}else{
		pthread_mutex_lock(&argControler->pmutexReadDataController->mutex);
		value=argControler->controllerStop;
		pthread_mutex_unlock(&argControler->pmutexReadDataController->mutex);
		return value;
	}
}

void control(args_CONTROLLER * argsControl);

void *thread_CONTROLLER(void *args) {

	args_CONTROLLER *argController =(args_CONTROLLER *) args;

	control( argController);

	signalController(argController);

	logString("THREAD CONTROLLER : END");

	return NULL;
}

int init_args_CONTROLLER(args_CONTROLLER ** argController,volatile sig_atomic_t * boolStopController){

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

	*argController =(args_CONTROLLER *) malloc(sizeof(args_CONTROLLER));
	if (*argController == NULL) {
		logString("MALLOC FAIL : argController");
		return EXIT_FAILURE;
	}
	(*argController)->newThing=0;
	(*argController)->controllerStop=0;
	(*argController)->manette=(DataController *) malloc(sizeof( DataController));
	if ((*argController)->manette == NULL) {
		logString("MALLOC FAIL : argController->manette");
		return EXIT_FAILURE;
	}
	(*argController)->manette->flag=0;
	(*argController)->manette->axe_FrontBack=0;
	(*argController)->manette->axe_LeftRight=0;
	(*argController)->manette->axe_Rotation=0;
	(*argController)->manette->axe_UpDown=0;
	(*argController)->pmutexReadDataController=pmutexRead;
	(*argController)->pmutexControllerPlug=pmutexControllerPlug;

	(*argController)->boolStopController=boolStopController;

	return 0;
}


void clean_args_CONTROLLER(args_CONTROLLER * arg) {
	if (arg != NULL) {
		clean_PMutex(arg->pmutexControllerPlug);
		clean_PMutex(arg->pmutexReadDataController);
		clean_DataController(arg->manette);
		free(arg);
		arg = NULL;
	}
}

int isControllerConnect=0;
inputt input;

char is_connect() {
	//char name[100];
	//strcpy(name, "Microsoft X-Box 360 pad");

	if (isControllerConnect == 0) {
		initialiserInput(&input, 0); // on l'initialise au joystick n°0
	}

	const char * tmp = SDL_JoystickName(0);
	if (tmp != NULL) {
		if(isControllerConnect==0){
			isControllerConnect=1;
			char array[SIZE_MAX_LOG];
			sprintf(array, "THREAD CONTROLLER : CONTROLLER CONNECT : %s", tmp);
			logString(array);
		}

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

void control(args_CONTROLLER * argsControl) {
	DataController * manette = argsControl->manette;
	int local_period=(1.0/FREQUENCY_CONTROLLER)*USEC_TO_SEC;
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

	int modele;
	int firstConnectMade=0;
	int quitter=0;

	if ((SDL_NumJoysticks() <= 0)){
		quitter=1;
		logString("THREAD CONTROLLER : ERROR no Controller plug");
	}
	float tmpM0, tmpM1, tmpM2, tmpM3;
	while (!quitter) {

		if(is_Controller_Stop(argsControl)){
			quitter=1;
			break;
		}

		if (!is_connect()) {
			char array[SIZE_MAX_LOG];

			if (firstConnectMade) {
				pthread_mutex_lock(&argsControl->pmutexReadDataController->mutex);
				manette->flag = 1;
				argsControl->newThing = 1;
				manette->axe_Rotation = 0;
				manette->axe_UpDown = 0;
				manette->axe_LeftRight = 0;
				manette->axe_FrontBack = 0;

				pthread_cond_signal(&argsControl->pmutexReadDataController->condition);
				pthread_mutex_unlock(&argsControl->pmutexReadDataController->mutex);
			}

			if (isControllerConnect == 1) {
				pthread_mutex_lock(&argsControl->pmutexReadDataController->mutex);
				manette->flag = 1;
				pthread_mutex_unlock(&argsControl->pmutexReadDataController->mutex);
				sprintf(array,"THREAD CONTROLLER : ERROR NO MORE Controller %d",manette->flag);
				logString(array);
				isControllerConnect = 0;
				printf("isControllerConnect : %d\n",isControllerConnect);
				detruireInput(&input);
			}




		}else{
			if(firstConnectMade){
				pthread_mutex_lock(&argsControl->pmutexReadDataController->mutex);
				manette->flag = 2;
				pthread_mutex_unlock(&argsControl->pmutexReadDataController->mutex);
			}
		}

		usleep(local_period);

		if(!isControllerConnect){
			continue;
		}

		updateEvent(&input); // on récupère les évènements

		if (input.boutons[4] && input.boutons[5]
				&& ((input.boutons[6] && input.boutons[7])
						|| (input.boutons[9] && input.boutons[10]))) {

			modele = !(input.boutons[9] && input.boutons[10]);
			//modele = 1 pour xbox controller, 0 for ps4 controller

			//TODO choix combinaison demarrage et arret

			int tmpFlag;

			pthread_mutex_lock(&argsControl->pmutexReadDataController->mutex);

			if (manette->flag == 2) {
				manette->flag = 0;
			} else {
				manette->flag = 2;
				firstConnectMade=1;
			}

			tmpFlag=manette->flag;
			pthread_mutex_unlock(&argsControl->pmutexReadDataController->mutex);

			input.boutons[4] = input.boutons[5] = input.boutons[6] =
					input.boutons[7] = input.boutons[9] = input.boutons[10] = 0;


			char array[SIZE_MAX_LOG];
			sprintf(array,"THREAD CONTROLLER : FLAG -> %d", tmpFlag);
			logString(array);


			while (!modele
					&& (input.axes[4] != -XBOX_CONTROLLER_MAX_VALUE || input.axes[5] != -XBOX_CONTROLLER_MAX_VALUE)) {
				updateEvent(&input);
			}
			sleep(1);
			signalController(argsControl);
		}


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
					pourcent(input.axes[0], XBOX_CONTROLLER_MAX_VALUE) :
					pourcent(input.axes[2], XBOX_CONTROLLER_MAX_VALUE);

			//tmpM1 = monter ou descendre (UpDown)
			tmpM1 = modele ?
					pourcent(-1 * input.axes[1], XBOX_CONTROLLER_MAX_VALUE) :
					(diff_axes(input.axes[5], input.axes[4], XBOX_CONTROLLER_MAX_VALUE));

			//tmpM2 = rotation axe roulis (LeftRight) (z)
			tmpM2 = modele ?
					pourcent(input.axes[3], XBOX_CONTROLLER_MAX_VALUE) :
					pourcent(input.axes[0], XBOX_CONTROLLER_MAX_VALUE);

			//tmpM2 = rotation axe tangage (FrontBack) (x)
			tmpM3 = modele ?
					pourcent(-1 * input.axes[4], XBOX_CONTROLLER_MAX_VALUE) :
					pourcent(-1 * input.axes[1], XBOX_CONTROLLER_MAX_VALUE);

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

	/* diverses destruction ...*/
	detruireInput(&input); // on détruit la structure input
	SDL_Quit(); // on quitte la SDL
}
