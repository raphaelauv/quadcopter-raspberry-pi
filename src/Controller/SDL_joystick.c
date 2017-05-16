#include "SDL_joystick.h"

void init_inputSDLjoystick(inputSDLjoystick *input, int numeroJoystick) {
	if (numeroJoystick < SDL_NumJoysticks()) // on vérifie qu'il y a bien un bon numéro de joystick
			{
		SDL_JoystickEventState(SDL_ENABLE);
		input->joystick = SDL_JoystickOpen(numeroJoystick); // on met le joystick à numéro correspondant
		input->numero = numeroJoystick; // je pense que vous comprenez cette ligne...

		/* on alloue chaque éléments en fonctions de combien il y en a */

		input->boutons = NULL;
		input->axes = NULL;
		input->chapeaux = NULL;
		input->trackballs = NULL;

		input->boutons = (char*) malloc(SDL_JoystickNumButtons(input->joystick) * sizeof(char));
		if(input->boutons == NULL){
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> boutons");	
		}
		input->axes = (int*) malloc(SDL_JoystickNumAxes(input->joystick) * sizeof(int));
		if(input->axes == NULL){
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> axes");	
		}
		input->chapeaux = (int*) malloc(SDL_JoystickNumHats(input->joystick) * sizeof(int));
		if(input->chapeaux == NULL){
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> chapeaux");	
		}
		input->trackballs = (inputTrackball*) malloc(SDL_JoystickNumBalls(input->joystick)* sizeof(inputTrackball));
		if(input->trackballs == NULL){
			logString("MALLOC FAIL : init_inputSDLjoystick SDL_JoystickNumButtons -> trackballs");	
		}


		for (int i = 0; i < SDL_JoystickNumButtons(input->joystick); i++) // tant qu'on a pas atteint le nombre max de boutons
			input->boutons[i] = 0; // on met les valeurs à 0
		for (int i = 0; i < SDL_JoystickNumAxes(input->joystick); i++) // tant qu'on a pas atteint le nombre max d'axes
			input->axes[i] = 0; // on met aussi les valeurs à 0
		for (int i = 0; i < SDL_JoystickNumHats(input->joystick); i++) // tant qu'on a pas atteint le nombre max de chapeaux
			input->chapeaux[i] = SDL_HAT_CENTERED; // on dit que les chapeaux son centrés
		for (int i = 0; i < SDL_JoystickNumBalls(input->joystick); i++) // tant qu'il y a des trackballs
				{
			// on met à zéro
			input->trackballs[i].xrel = 0;
			input->trackballs[i].yrel = 0;
		}
	}

	else {
		// si le numéro du joystick n'était pas correct
		// on met tout à NULL
		input->joystick = NULL;
		input->boutons = NULL;
		input->axes = NULL;
		input->chapeaux = NULL;
		input->trackballs = NULL;
	}
}
void clean_inputSDLjoystick(inputSDLjoystick *input) {
	if (input->joystick != NULL) // on vérifie que le joystick existe bien
	{
		input->numero = 0; // on le remet à zéro

		// on libère tout
		free(input->boutons);
		free(input->axes);
		free(input->chapeaux);
		free(input->trackballs);
		SDL_JoystickClose(input->joystick);
	}
}

void updateEvent(inputSDLjoystick *input) {
	static SDL_Event evenements; // en statique car appelle plusieurs fois par seconde
	for (int i = 0; i < SDL_JoystickNumBalls(input->joystick); i++) {
		//on met à zéro
		input->trackballs[i].xrel = 0;
		input->trackballs[i].yrel = 0;
	}

	while (SDL_PollEvent(&evenements)) // tant qu'il y a des évènements à traiter
	{
		if (input->joystick != NULL                    //si le joystick existe
				&& (evenements.jbutton.which == input->numero
						|| evenements.jaxis.which == input->numero // et si c'est bien je joystick a gérer
						|| evenements.jhat.which == input->numero
						|| evenements.jball.which == input->numero)) {
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
