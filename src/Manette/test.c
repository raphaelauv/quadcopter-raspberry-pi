#include "manette.h"

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK); // on initialise les sous-programmes vidéo et joystick

	struct Input input; // on crée la structure
	initialiserInput(&input, 0); // on l'initialise au joystick n°0

	int quitter = 0;

	while (!quitter) {
		updateEvent(&input); // on récupère les évènements
		sleep(1);
		/* traitement des événements (à suivre) ... */
		printf("Axes 1: %d,%d     -       Axes 2: %d,%d \n ", input.axes[0],
				input.axes[1], input.axes[2], input.axes[3]);

	}

	/* diverses destruction ...*/

	detruireInput(&input); // on détruit la structure input
	SDL_Quit(); // on quitte la SDL

	return 0;
}

