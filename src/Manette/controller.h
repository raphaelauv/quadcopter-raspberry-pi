#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "manette.h"

#define Update_Frequence 1

typedef struct dataController{
	char moteur_active;
	float moteur0;
	float moteur1;
	float moteur2;
	float moteur3;
}dataController;

void control(dataController * manette);

#endif /* CONTROLLER_H_ */
