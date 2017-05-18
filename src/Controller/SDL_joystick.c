#include "joystick.h"
#include <SDL/SDL.h>
#include <SDL/SDL_events.h>
//TODO SDL2 ne fait pas fonctionner la manette xbox360 , a corrigé
//#include <SDL2/SDL_events.h>
//#include <SDL2/SDL.h>


int init_inputJoystick(inputJoystick *input, int joystickNumber) {
	if (joystickNumber < SDL_NumJoysticks()) // on vérifie qu'il y a bien un bon numéro de joystick
			{
		SDL_JoystickEventState(SDL_ENABLE);
		input->joystick = SDL_JoystickOpen(joystickNumber); // on met le joystick à numéro correspondant
		input->number = joystickNumber; // je pense que vous comprenez cette ligne...

		SDL_Joystick * sdlJoystick =(SDL_Joystick *) input->joystick;
		/* on alloue chaque éléments en fonctions de combien il y en a */

		input->boutons = NULL;
		input->axes = NULL;
		input->chapeaux = NULL;
		input->trackballs = NULL;

		int error=0;
		input->boutons = (char*) malloc(SDL_JoystickNumButtons(sdlJoystick) * sizeof(char));
		if(input->boutons == NULL){
			error=1;
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> boutons");	
		}
		input->axes = (int*) malloc(SDL_JoystickNumAxes(sdlJoystick) * sizeof(int));
		if(input->axes == NULL){
			error=1;
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> axes");	
		}
		input->chapeaux = (int*) malloc(SDL_JoystickNumHats(sdlJoystick) * sizeof(int));
		if(input->chapeaux == NULL){
			error=1;
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> chapeaux");	
		}
		input->trackballs = (inputTrackball*) malloc(SDL_JoystickNumBalls(sdlJoystick)* sizeof(inputTrackball));
		if(input->trackballs == NULL){
			error=1;
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> trackballs");	
		}

		if(error){
			//TODO gotoClean
			return -1;
		}

		for (int i = 0; i < SDL_JoystickNumButtons(sdlJoystick); i++) // tant qu'on a pas atteint le nombre max de boutons
			input->boutons[i] = 0; // on met les valeurs à 0
		for (int i = 0; i < SDL_JoystickNumAxes(sdlJoystick); i++) // tant qu'on a pas atteint le nombre max d'axes
			input->axes[i] = 0; // on met aussi les valeurs à 0
		for (int i = 0; i < SDL_JoystickNumHats(sdlJoystick); i++) // tant qu'on a pas atteint le nombre max de chapeaux
			input->chapeaux[i] = SDL_HAT_CENTERED; // on dit que les chapeaux son centrés
		for (int i = 0; i < SDL_JoystickNumBalls(sdlJoystick); i++) // tant qu'il y a des trackballs
				{
			// on met à zéro
			input->trackballs[i].xrel = 0;
			input->trackballs[i].yrel = 0;
		}
		return 0;
	}

	else {
		// si le numéro du joystick n'était pas correct
		// on met tout à NULL
		input->joystick = NULL;
		input->boutons = NULL;
		input->axes = NULL;
		input->chapeaux = NULL;
		input->trackballs = NULL;
		return -1;
	}
}
void clean_inputJoystick(inputJoystick *input) {
	if (input->joystick != NULL) // on vérifie que le joystick existe bien
	{
		input->number = 0; // on le remet à zéro
		SDL_Joystick * sdlJoystick =(SDL_Joystick *) input->joystick;

		// on libère tout
		free(input->boutons);
		free(input->axes);
		free(input->chapeaux);
		free(input->trackballs);
		SDL_JoystickClose(sdlJoystick);
	}
}

int update_eventJoystick(inputJoystick *input) {
	static SDL_Event evenements; // en statique car appelle plusieurs fois par seconde

	SDL_Joystick * sdlJoystick =(SDL_Joystick *) input->joystick;

	for (int i = 0; i < SDL_JoystickNumBalls(sdlJoystick); i++) {
		//on met à zéro
		input->trackballs[i].xrel = 0;
		input->trackballs[i].yrel = 0;
	}

	while (SDL_PollEvent(&evenements)) // tant qu'il y a des évènements à traiter
	{
		if (sdlJoystick != NULL                    //si le joystick existe
				&& (evenements.jbutton.which == input->number
						|| evenements.jaxis.which == input->number // et si c'est bien je joystick a gérer
						|| evenements.jhat.which == input->number
						|| evenements.jball.which == input->number)) {
			switch (evenements.type) {
			case SDL_JOYBUTTONDOWN:
				input->boutons[evenements.jbutton.button] = 1; // bouton appuyé : valeur du bouton : 1
				break;

			case SDL_JOYBUTTONUP:
				input->boutons[evenements.jbutton.button] = 0; // bouton relâché : valeur du bouton : 0
				break;

			case SDL_JOYAXISMOTION:
				input->axes[evenements.jaxis.axis] = evenements.jaxis.value;
				break;

			case SDL_JOYHATMOTION:
				input->chapeaux[evenements.jhat.hat] = evenements.jhat.value;
				break;

			case SDL_JOYBALLMOTION:
				input->trackballs[evenements.jball.ball].xrel =
						evenements.jball.xrel;
				input->trackballs[evenements.jball.ball].yrel =
						evenements.jball.yrel;
				break;

			default:
				break;
			}
		}
	}
}


char * isConnect_Joystick(int number){
	return (char *) SDL_JoystickName(number);
}

int init_Joystick(){
	return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK); // on initialise les sous-programmes vidéo et joystick
}

int numberOfConnected_Joystick(){
	return SDL_NumJoysticks();
}

void clean_Joystick(){
	SDL_Quit(); // quit SDL
}