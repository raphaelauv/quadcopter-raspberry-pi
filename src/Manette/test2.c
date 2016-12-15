#include <SDL/SDL.h>

int main2() {
	// Démarre SDL avec la gestion des joysticks
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1) {
		fprintf(stderr, "Erreur lors de l'initialisation de la SDL\n");
		return -1;
	}

	SDL_Joystick* pJoystick = NULL;
	int numJoystick = SDL_NumJoysticks(); // Compte le nombre de joysticks
	printf("Vous avez %d joysticks sur cette machine\n", numJoystick);
	if (numJoystick >= 1) {
		// Ouvre le premier joystick présent sur la machine
		pJoystick = SDL_JoystickOpen(0);
		if (pJoystick == NULL) {
			fprintf(stderr, "Erreur pour ouvrir le premier joystick\n");
		}
	}
	printf("Informations du joystick\n");
	printf("Nom : %s\n", SDL_JoystickName(0));
	printf("Nombre d'axes : %d\n", SDL_JoystickNumAxes(pJoystick));
	printf("Nombre de chapeaux : %d\n", SDL_JoystickNumHats(pJoystick));
	printf("Nombre de trackballs : %d\n", SDL_JoystickNumBalls(pJoystick));
	printf("Nombre de boutons : %d\n", SDL_JoystickNumButtons(pJoystick));
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_Event event;
	while (1) {
		SDL_WaitEvent(&event);
		if (event.type == SDL_JOYBUTTONDOWN) {
			printf("Bouton : %d\n", event.jbutton.button);
		}

		if (event.type == SDL_JOYAXISMOTION) {
			// Mouvemen
			int i = event.jaxis.value, j = event.jaxis.axis;
			if (j == 4)
				printf("%d,%d\n", i, j);
		}
	}
	SDL_JoystickClose(pJoystick);
}
