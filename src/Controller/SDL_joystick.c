#include "joystick.h"
#include <SDL/SDL.h>
#include <SDL/SDL_events.h>
#include "../concurrent.h"

// Thank to this tutoriel->https://openclassrooms.com/courses/la-gestion-du-joystick-avec-la-sdl;

//TODO SDL2 ne fait pas fonctionner la manette xbox360 , a corrigé
//#include <SDL2/SDL_events.h>
//#include <SDL2/SDL.h>


void init_inputJoystick(inputJoystick *input) {

	input->axes = NULL;
	input->boutons = NULL;
	input->chapeaux = NULL;
	input->joystick = NULL;
	input->trackballs = NULL;
	input->number = 0;
}

int update_inputJoystick(inputJoystick *input, int joystickNumber) {

	if(input==NULL){
		return -1;
	}

	if (joystickNumber < SDL_NumJoysticks()){ // tcheck the number of joysticks
			
		SDL_JoystickEventState(SDL_ENABLE);

		clean_inputJoystick(input);

		input->joystick = SDL_JoystickOpen(joystickNumber);
		if(input->joystick == NULL){
			// the joyStick is not ready
			goto cleanFail;
		}
		input->number = joystickNumber;

		SDL_Joystick * sdlJoystick =(SDL_Joystick *) input->joystick;


		input->boutons = (char*) malloc(SDL_JoystickNumButtons(sdlJoystick) * sizeof(char));
		if(input->boutons == NULL){
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> boutons");
			goto cleanFail;
		}
		input->axes = (int*) malloc(SDL_JoystickNumAxes(sdlJoystick) * sizeof(int));
		if(input->axes == NULL){
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> axes");
			goto cleanFail;
		}
		input->chapeaux = (int*) malloc(SDL_JoystickNumHats(sdlJoystick) * sizeof(int));
		if(input->chapeaux == NULL){
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> chapeaux");
			goto cleanFail;
		}
		input->trackballs = (inputTrackball*) malloc(SDL_JoystickNumBalls(sdlJoystick)* sizeof(inputTrackball));
		if(input->trackballs == NULL){
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> trackballs");
			goto cleanFail;
		}


		for (int i = 0; i < SDL_JoystickNumButtons(sdlJoystick); i++){
			// tant qu'on a pas atteint le nombre max de boutons
			input->boutons[i] = 0;
		}
		for (int i = 0; i < SDL_JoystickNumAxes(sdlJoystick); i++){
			// tant qu'on a pas atteint le nombre max d'axes
			input->axes[i] = 0;
		}
		for (int i = 0; i < SDL_JoystickNumHats(sdlJoystick); i++){
			// tant qu'on a pas atteint le nombre max de chapeaux
			input->chapeaux[i] = SDL_HAT_CENTERED; // on dit que les chapeaux son centrés
		}
		for (int i = 0; i < SDL_JoystickNumBalls(sdlJoystick); i++){
			// tant qu'il y a des trackballs , on met à zéro
			input->trackballs[i].xrel = 0;
			input->trackballs[i].yrel = 0;
		}
		return 0;
	}

cleanFail:
	clean_inputJoystick(input);
	return -1;

}
void clean_inputJoystick(inputJoystick *input) {
	if (input != NULL){
	
		if(input->boutons!=NULL){
			free(input->boutons);	
		}
		if(input->axes!=NULL){
			free(input->axes);	
		}
		if(input->chapeaux!=NULL){
			free(input->chapeaux);	
		}
		if(input->trackballs!=NULL){
			free(input->trackballs);	
		}

		SDL_Joystick * sdlJoystick =(SDL_Joystick *) input->joystick;
		if(sdlJoystick!=NULL){
			SDL_JoystickClose(sdlJoystick);	
		}

		init_inputJoystick(input);

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
