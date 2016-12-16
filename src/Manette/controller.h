#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "manette.h"
#include <string.h>
#define Update_Frequence 20

struct controller{
	char moteur_active;
	float moteur0;
	float moteur1;
	float moteur2;
	float moteur3;
};

void controller(struct controller * manette);
