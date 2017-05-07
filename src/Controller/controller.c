#include "controller.h"


void signalController(args_CONTROLLER * argsControl){
	pthread_mutex_lock(&argsControl->pmutexControllerPlug->mutex);
	pthread_cond_signal(&argsControl->pmutexControllerPlug->condition);
	pthread_mutex_unlock(&argsControl->pmutexControllerPlug->mutex);

}

void set_Controller_Stop(args_CONTROLLER * argControler){
	pthread_mutex_lock(&argControler->pmutex->mutex);
	argControler->controllerStop=1;
	pthread_mutex_unlock(&argControler->pmutex->mutex);
}

int is_Controller_Stop(args_CONTROLLER * argControler){
	//first look to glabal signal value
	int value=*(argControler->boolStopController);
	if(value){
		set_Controller_Stop(argControler);
		return value;

	//or look to the atomic value
	}else{
		pthread_mutex_lock(&argControler->pmutex->mutex);
		value=argControler->controllerStop;
		pthread_mutex_unlock(&argControler->pmutex->mutex);
		return value;
	}
}

void control(args_CONTROLLER * argsControl);

void *thread_CONTROLLER(void *args) {

	args_CONTROLLER *argController =(args_CONTROLLER *) args;

	control( argController);

	set_Controller_Stop(argController);

	signalController(argController);

	logString("THREAD CONTROLLER : END");

	return NULL;
}

int init_args_CONTROLLER(args_CONTROLLER ** argController,volatile sig_atomic_t * boolStopController){


	DataController * dataController;
	PMutex * pmutexControllerPlug;
	PMutex * pmutexArg_Controller;
	PMutex * pmutexController;

	*argController = (args_CONTROLLER *) malloc(sizeof(args_CONTROLLER));
	if (*argController == NULL) {
		logString("MALLOC FAIL : argController");
		return -1;
	}


	(*argController)->controllerStop=0;
	(*argController)->boolStopController=boolStopController;

	pmutexControllerPlug =(PMutex *) malloc(sizeof(PMutex));
	if (pmutexControllerPlug == NULL) {
		logString("MALLOC FAIL : pmutexControllerPlug");
		goto cleanFail;
	}
	(*argController)->pmutexControllerPlug=pmutexControllerPlug;
	init_PMutex(pmutexControllerPlug);


	pmutexArg_Controller =(PMutex *) malloc(sizeof(PMutex));
	if (pmutexArg_Controller == NULL) {
		logString("MALLOC FAIL : pmutexRead");
		goto cleanFail;
	}
	(*argController)->pmutex=pmutexArg_Controller;
	init_PMutex(pmutexArg_Controller);

	dataController=(DataController *) malloc(sizeof( DataController));
	if (dataController == NULL) {
		logString("MALLOC FAIL : dataController");
		goto cleanFail;
	}
	(*argController)->dataControl=dataController;

	pmutexController = (PMutex *) malloc(sizeof(PMutex));
	if (pmutexController == NULL) {
		logString("MALLOC FAIL : pmutexController");
		goto cleanFail;
	}
	dataController->pmutex = pmutexController;
	init_PMutex(pmutexController);

	pthread_mutex_lock(&dataController->pmutex->mutex);

	dataController->flag=0;
	dataController->axe_FrontBack=0;
	dataController->axe_LeftRight=0;
	dataController->axe_Rotation=0;
	dataController->axe_UpDown=0;

	pthread_mutex_unlock(&dataController->pmutex->mutex);

	return 0;

cleanFail:

	clean_args_CONTROLLER(*argController);
	return -1;
}


void clean_args_CONTROLLER(args_CONTROLLER * arg) {
	if (arg != NULL) {
		clean_PMutex(arg->pmutexControllerPlug);
		clean_PMutex(arg->pmutex);
		clean_DataController(arg->dataControl);
		free(arg);
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
	DataController * dataControl = argsControl->dataControl;
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

	float manualTmpM1=0;

	while (!quitter) {

		if(is_Controller_Stop(argsControl)){
			quitter=1;
			break;
		}

		if (!is_connect()) {
			char array[SIZE_MAX_LOG];

			if (firstConnectMade) {


				pthread_mutex_lock(&dataControl->pmutex->mutex);
				dataControl->flag = 1;
				dataControl->axe_Rotation = 0;
				dataControl->axe_UpDown = 0;
				dataControl->axe_LeftRight = 0;
				dataControl->axe_FrontBack = 0;

				pthread_cond_signal(&dataControl->pmutex->condition);
				pthread_mutex_unlock(&dataControl->pmutex->mutex);
			}

			if (isControllerConnect == 1) {
				pthread_mutex_lock(&dataControl->pmutex->mutex);
				dataControl->flag = 1;
				pthread_mutex_unlock(&dataControl->pmutex->mutex);
				sprintf(array,"THREAD CONTROLLER : ERROR NO MORE Controller %d",dataControl->flag);
				logString(array);
				isControllerConnect = 0;
				printf("isControllerConnect : %d\n",isControllerConnect);
				detruireInput(&input);
			}




		}else{
			if(firstConnectMade){
				pthread_mutex_lock(&dataControl->pmutex->mutex);
				dataControl->flag = 2;
				pthread_mutex_unlock(&dataControl->pmutex->mutex);
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

			pthread_mutex_lock(&dataControl->pmutex->mutex);

			if (dataControl->flag == 2) {
				dataControl->flag = 0;
			} else {
				dataControl->flag = 2;
				firstConnectMade=1;
			}

			tmpFlag=dataControl->flag;
			pthread_mutex_unlock(&dataControl->pmutex->mutex);

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

			/*
			if(tmpM1>50){
				manualTmpM1++;
			}

			if(tmpM1<-50){
				manualTmpM1-=2;
			}
			*/

			//tmpM2 = rotation axe roulis (LeftRight) (z)
			tmpM2 = modele ?
					pourcent(input.axes[3], XBOX_CONTROLLER_MAX_VALUE) :
					pourcent(input.axes[0], XBOX_CONTROLLER_MAX_VALUE);

			//tmpM2 = rotation axe tangage (FrontBack) (x)
			tmpM3 = modele ?
					pourcent(-1 * input.axes[4], XBOX_CONTROLLER_MAX_VALUE) :
					pourcent(-1 * input.axes[1], XBOX_CONTROLLER_MAX_VALUE);

			pthread_mutex_lock(&dataControl->pmutex->mutex);


			dataControl->axe_Rotation = tmpM0;
			dataControl->axe_UpDown = tmpM1;
			//manette->axe_UpDown = manualTmpM1;
			dataControl->axe_LeftRight = tmpM2;
			dataControl->axe_FrontBack = tmpM3;

			pthread_cond_signal(&dataControl->pmutex->condition);
			pthread_mutex_unlock(&dataControl->pmutex->mutex);

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
