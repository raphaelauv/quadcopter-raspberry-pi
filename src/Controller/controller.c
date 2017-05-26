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
	int value=*(argControler->signalControllerStop);
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

int init_args_CONTROLLER(args_CONTROLLER ** argController,volatile sig_atomic_t * signalControllerStop){


	DataController * dataController = NULL;
	PMutex * pmutexControllerPlug = NULL;
	PMutex * pmutexArg_Controller = NULL;
	PMutex * pmutexController = NULL;

	*argController = (args_CONTROLLER *) malloc(sizeof(args_CONTROLLER));
	if (*argController == NULL) {
		logString("MALLOC FAIL : argController");
		return -1;
	}

	(*argController)->controllerStop=0;
	(*argController)->signalControllerStop=signalControllerStop;

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
	*argController=NULL;
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


//TODO no need for a global value
int isControllerConnect=0;

int is_connect(inputJoystick * input) {
	//char name[100];
	//strcpy(name, "Microsoft X-Box 360 pad");

	if (!isControllerConnect) {
		update_inputJoystick(input, 0); // on l'initialise au joystick n°0
	}

	char * tmp = isConnect_Joystick(0);
	if (tmp != NULL) {
		if(!isControllerConnect){
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

	int modele;
	int firstConnectMade = 0;
	int quitter = 0;
	inputJoystick input;

	init_inputJoystick(&input);

	float lastPowerASK=0;

	float arrayValController[CONTROLLER_NUMBER_AXES];
	float arrayValAsk[CONTROLLER_NUMBER_AXES];

	for(int i=0;i<CONTROLLER_NUMBER_AXES;i++){
		arrayValController[i]=0;
		arrayValAsk[i]=0;
	}

	if(init_Joystick()){
		quitter=1;
	}else{
		if (numberOfConnected_Joystick() <= 0) {
			quitter = 1;
			logString("THREAD CONTROLLER : ERROR no Controller plug");
		}
	}

	while (!quitter) {

		if(is_Controller_Stop(argsControl)){
			quitter=1;
			break;
		}

		if (!is_connect(&input)) {
			char array[SIZE_MAX_LOG];

			if (firstConnectMade) {

				//PAUSE MSG
				pthread_mutex_lock(&dataControl->pmutex->mutex);
				dataControl->flag = 1;
				dataControl->axe_Rotation = 0;
				dataControl->axe_UpDown = 0;
				dataControl->axe_LeftRight = 0;
				dataControl->axe_FrontBack = 0;

				pthread_cond_signal(&dataControl->pmutex->condition);
				pthread_mutex_unlock(&dataControl->pmutex->mutex);
			}

			// Controller have just been disconnected
			if (isControllerConnect == 1) {

				pthread_mutex_lock(&dataControl->pmutex->mutex);
				dataControl->flag = 1;// TODO not necessery
				pthread_mutex_unlock(&dataControl->pmutex->mutex);

				sprintf(array,"THREAD CONTROLLER : ERROR NO MORE Controller %d",dataControl->flag);
				logString(array);
				isControllerConnect = 0;
				//printf("isControllerConnect : %d\n",isControllerConnect);
				clean_inputJoystick(&input);
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

		update_eventJoystick(&input); // on récupère les évènements

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
					&& (input.axes[4] != -JOYSTICK_MAX_VALUE || input.axes[5] != -JOYSTICK_MAX_VALUE)) {
				update_eventJoystick(&input);
			}
			sleep(1);
			signalController(argsControl);
		}


			//tmpM0 = Rotation axe lacet (Rotation) (y)
		arrayValController[0] = modele ?
					pourcent(input.axes[0], JOYSTICK_MAX_VALUE) :
					pourcent(input.axes[2], JOYSTICK_MAX_VALUE);


			//tmpM1 = monter ou descendre (UpDown)
		arrayValController[1] = modele ?
					pourcent(-1 * input.axes[1], JOYSTICK_MAX_VALUE) :
					(diff_axes(input.axes[5], input.axes[4], JOYSTICK_MAX_VALUE));


			//tmpM2 = rotation axe roulis (LeftRight) (z)
			arrayValController[2] = modele ?
					pourcent(input.axes[3], JOYSTICK_MAX_VALUE) :
					pourcent(input.axes[0], JOYSTICK_MAX_VALUE);

			//tmpM2 = rotation axe tangage (FrontBack) (x)
			arrayValController[3] = modele ?
					pourcent(-1 * input.axes[4], JOYSTICK_MAX_VALUE) :
					pourcent(-1 * input.axes[1], JOYSTICK_MAX_VALUE);



			/******************CONTROLLER FLANGE***********************/

			for(int i =0; i<CONTROLLER_NUMBER_AXES;i++){

				if(i==1){
					if(CONTROLLER_UP_DOWN_VARIABLE_MODE){
						continue;
					}
				}

				if (arrayValController[i] > CONTROLLER_LIMIT_PRECISION) {
					if (arrayValController[i] >= arrayValAsk[i]) {
						arrayValAsk[i] += (arrayValController[i] - arrayValAsk[i])/ CONTROLLER_FLANGE;
					}else{
						arrayValAsk[i] -= (arrayValAsk[i] - arrayValController[i] )/ CONTROLLER_FLANGE;
					}
				}


				else if(arrayValController[i]< - CONTROLLER_LIMIT_PRECISION){
					if (arrayValController[i] <= arrayValAsk[i]) {
					arrayValAsk[i] += (arrayValController[i] - arrayValAsk[i])/ CONTROLLER_FLANGE;
					}else{
						arrayValAsk[i] -= (arrayValAsk[i] - arrayValController[i] )/ CONTROLLER_FLANGE;
					}
				}

				else{

					if (arrayValAsk[i] == 0 || (arrayValAsk[i] < CONTROLLER_FLANGE && arrayValAsk[i] > -CONTROLLER_FLANGE)) {
						arrayValAsk[i] = 0;
					} else if (arrayValAsk[i] >= CONTROLLER_FLANGE) {
						arrayValAsk[i] -= CONTROLLER_FLANGE;
					} else if (arrayValAsk[i] <= -CONTROLLER_FLANGE) {
						arrayValAsk[i] += CONTROLLER_FLANGE;
					}
				}


				if(arrayValAsk[i]>99){
					arrayValAsk[i]=CONTROLLER_MAX;
				}

				if(arrayValAsk[i]<-99){
					arrayValAsk[i]=-CONTROLLER_MAX;
				}


			}

			for(int i =0; i<CONTROLLER_NUMBER_AXES;i++){
				if(i==1){
					//NO limition for axe_UpDown , but variable use

					if(CONTROLLER_UP_DOWN_VARIABLE_MODE){
						if(arrayValController[i]>CONTROLLER_LIMIT_PRECISION){
							lastPowerASK+=2;
						}else if(arrayValController[i]< - CONTROLLER_LIMIT_PRECISION){
							lastPowerASK-=2;
						}

						arrayValAsk[i]=lastPowerASK;
					}
				}else{
					if(arrayValAsk[i]>0){
						if(arrayValAsk[i]>CONTROLLER_LIMIT){
							arrayValAsk[i]=CONTROLLER_LIMIT;
						}
					}else{
						if (arrayValAsk[i] < - CONTROLLER_LIMIT) {
							arrayValAsk[i] = - CONTROLLER_LIMIT;
						}
					}
				}
			}

			/******************************************************/

			pthread_mutex_lock(&dataControl->pmutex->mutex);

			dataControl->axe_Rotation = arrayValAsk[0];
			dataControl->axe_UpDown = arrayValAsk[1];
			dataControl->axe_LeftRight = arrayValAsk[2];
			dataControl->axe_FrontBack = arrayValAsk[3];

			pthread_cond_signal(&dataControl->pmutex->condition);
			pthread_mutex_unlock(&dataControl->pmutex->mutex);

			/*
				printf(
						"Axe_rotation: %f, axe_Up_Down: %f     -       Axes_LeftRight: %f, axe_FrontBack: %f \n ",
						manette->axe_Rotation, manette->axe_UpDown,
						manette->axe_LeftRight, manette->axe_FrontBack);
			*/

	}


	clean_inputJoystick(&input); // clean structur input
	clean_Joystick();
}
