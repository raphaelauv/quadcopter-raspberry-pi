#ifndef JOYSTICK_H_
#define JOYSTICK_H_

typedef struct inputTrackball{ //structure pour les trackballs qui ont 2 variables à prendre en compte
	int xrel;	// valeur x réelle
	int yrel;	// valeur y réelle
} inputTrackball;

typedef struct inputJoystick {
	void *joystick; 				// the joystick
	char *boutons; 					// the boutons
	int *axes; 						// les axes
	int *chapeaux; 					// les chapeaux
	int number; 					// number of joystick
	inputTrackball * trackballs; 	// les trackballs

} inputJoystick;

int init_Joystick();
void clean_Joystick();

int numberOfConnected_Joystick();
int init_inputJoystick(inputJoystick *input, int joystickNumber); 	// initialise en fonction du numéro du joystick
void clean_inputJoystick(inputJoystick *input); 					// free memory and close joystick 
int update_eventJoystick(inputJoystick *input);						// update the events of the joystick

char * isConnect_Joystick(int number);


#endif /* JOYSTICK_H_ */
