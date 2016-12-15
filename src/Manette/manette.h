#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>
// Copier sur cette adresse ->https://openclassrooms.com/courses/la-gestion-du-joystick-avec-la-sdl;

struct InputTrackball // je crée une structure car les trackballs ont 2 variables à prendre en compte

{
	int xrel; // valeur x réelle

	int yrel; // valeur y réelle
};

struct Input {

	SDL_Joystick *joystick; // le joystick

	char *boutons; // les boutons

	int *axes; // les axes

	int *chapeaux; // les chapeaux

	struct InputTrackball * trackballs; // les trackballs

	int numero; // le numero

};

void initialiserInput(struct Input *input, int numeroJoystick); // initialise en fonction du numéro du joystick

void detruireInput(struct Input *input); // libère la mémoire allouée

void updateEvent(struct Input *input); // récupère les évènements
