#ifndef MANETTE_H_
#define MANETTE_H

#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>


// Copier sur cette adresse ->https://openclassrooms.com/courses/la-gestion-du-joystick-avec-la-sdl;

typedef struct inputTrackball{ // je crée une structure car les trackballs ont 2 variables à prendre en compte

	int xrel; // valeur x réelle

	int yrel; // valeur y réelle
} inputTrackball;

typedef struct inputt {

	SDL_Joystick *joystick; // le joystick

	char *boutons; // les boutons

	int *axes; // les axes

	int *chapeaux; // les chapeaux

	inputTrackball * trackballs; // les trackballs

	int numero; // le numero

} inputt;

void initialiserInput(inputt *input, int numeroJoystick); // initialise en fonction du numéro du joystick

void detruireInput(inputt *input); // libère la mémoire allouée

void updateEvent(inputt *input); // récupère les évènements

int test1();

int test2();

#endif /* MANETTE_H */
