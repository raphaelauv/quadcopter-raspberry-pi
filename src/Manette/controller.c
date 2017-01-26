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


float pourcent(int valeur ,float reference){
  return ((float) valeur) * 100 / ((float)reference);
}

float diff_axes(int axe_down, int axe_up, int val_max){
  int val_down = axe_down + val_max;
  int val_up = axe_up + val_max;
  int val_general = val_up - val_down;
  return pourcent(val_general, val_max * 2);
}

void control(args_CONTROLER * argsControl) {
  DataController * manette = argsControl->manette;
  char verbose = argsControl->verbose;
  
  manette->flag = 0;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK); // on initialise les sous-programmes vidéo et joystick

  if(verbose){
    for(int j=0; j<SDL_NumJoysticks(); j++){
      printf("controller %d : %s\n", j, SDL_JoystickName(j));
    }
  }
  
  
  inputt input; // on crée la structure
  initialiserInput(&input, 0); // on l'initialise au joystick n°0
  
  int modele;
  int val_max = 32768;
  
  int quitter = (SDL_NumJoysticks()>0) ? 0 : 1;

  if(verbose && (SDL_NumJoysticks()<=0))
    printf("pas de controller\n");

      
  float tmpM0,tmpM1,tmpM2,tmpM3;
  while (!quitter){
    usleep(Update_Frequence);
    
    updateEvent(&input); // on récupère les évènements
    
    if(input.boutons[4] && input.boutons[5] && ( (input.boutons[6] && input.boutons[7]) || (input.boutons[9] && input.boutons[10]) ) ) {
      
      modele = !(input.boutons[9] && input.boutons[10]);
      //modele = 1 pour xbox controller, 0 for ps4 controller


      //TODO choix combinaison demarrage et arret

      if(manette->flag==2){
    	  manette->flag=0;
      }else{
    	  manette->flag=2;
      }

      input.boutons[4] = input.boutons[5] = input.boutons[6] =
	input.boutons[7] = input.boutons[9] = input.boutons[10] = 0;
      
      if(verbose){printf("bool_moteur_active= %d\n", manette->flag);}

			while (!modele
					&& (input.axes[4] != -val_max || input.axes[5] != -val_max)) {
				updateEvent(&input);
			}
      sleep(3);
      
      pthread_mutex_lock(&argsControl->pmutexControlerPlug->mutex);
      pthread_cond_signal(&argsControl->pmutexControlerPlug->condition);
      pthread_mutex_unlock(&argsControl->pmutexControlerPlug->mutex);
    }
    
    //printf("APRES IF\n");
    if (manette->flag==2) {
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
      manette->axe_Rotation =tmpM0;
      manette->axe_UpDown =tmpM1;
      manette->axe_LeftRight =tmpM2;
      manette->axe_FrontBack =tmpM3;
      
      pthread_mutex_unlock(&argsControl->pmutexReadDataController->mutex);
      
      if(verbose){printf("Axe_rotation: %f, axe_Up_Down: %f     -       Axes_LeftRight: %f, axe_FrontBack: %f \n ",
			 manette->axe_Rotation, manette->axe_UpDown, manette->axe_LeftRight,
			 manette->axe_FrontBack);}
      
      sleep(1);//TODO
    }
    
    
    if (!is_connect()) {
      manette->flag = 0;
      if(verbose){printf("Plus de manette %d.\n", manette->flag);}
      break;
    }
    
  }
  
  
  /* diverses destruction ...*/
  
  detruireInput(&input); // on détruit la structure input
  SDL_Quit(); // on quitte la SDL
  
  return;
}
