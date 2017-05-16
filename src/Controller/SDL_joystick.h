#ifndef SDL_joystick_H_
#define SDL_joystick_H

#include <SDL/SDL.h>
#include <SDL/SDL_events.h>
//TODO SDL2 ne fait pas fonctionner la manette xbox360 , a corrigé
//#include <SDL2/SDL_events.h>
//#include <SDL2/SDL.h>

#include "../concurrent.h"


// Copier sur cette adresse ->https://openclassrooms.com/courses/la-gestion-du-joystick-avec-la-sdl;

typedef struct inputTrackball{ // je crée une structure car les trackballs ont 2 variables à prendre en compte

	int xrel; // valeur x réelle

	int yrel; // valeur y réelle
} inputTrackball;

typedef struct inputSDLjoystick {

	SDL_Joystick *joystick; // le joystick

	char *boutons; // les boutons

	int *axes; // les axes

	int *chapeaux; // les chapeaux

	inputTrackball * trackballs; // les trackballs

	int numero; // le numero

} inputSDLjoystick;

void init_inputSDLjoystick(inputSDLjoystick *input, int numeroJoystick); // initialise en fonction du numéro du joystick

void clean_inputSDLjoystick(inputSDLjoystick *input); // libère la mémoire allouée

void updateEvent(inputSDLjoystick *input); // récupère les évènements


#endif /* SDL_joystick_H */
