//
//  Calibration_ESC.c
//  Drone
//
//  Created by andres quiroz on 25/01/2017.
//  Copyright © 2017 andres quiroz. All rights reserved.
//

#include "Calibration_ESC.h"


void viderBuffer(void){
    int c;
    while((c=getchar()) != EOF && c != '\n');
}

void sleepDuration(int sleepTime) {
	int result = -1;
	while (result != 0) {
		result = sleep(sleepTime);
		sleepTime -= result;
	}
}

void * calibrate(void * args){
    args_CONTROLDEVOL  * controle_vol =(args_CONTROLDEVOL *)args;
    viderBuffer();
    if(controle_vol->verbose){
    	printf("Calibration Des ESC:\n Pour calibrer les ESCs Brancher la baterie "
    			"APRES AVOIR LANCEE CE PROGRAMME.\nPuis Atender les Deux beep des ESCs et "
    			"appuyer sur o \n Apres cela aller atendre les 3 beep des ESCs, Vos ESC son calibre.\n");
    }

    //PUT 100% of power.
    set_power(controle_vol->motorsAll->motor0,10.0);
    set_power(controle_vol->motorsAll->motor1,10.0);
    set_power(controle_vol->motorsAll->motor2,10.0);
    set_power(controle_vol->motorsAll->motor3,10.0);
    char a;
    if(controle_vol->verbose){

    	scanf("%c", &a);
    	viderBuffer();
    }else{
    	sleepDuration(5);
    }
    //PUT 0% of power.
    set_power(controle_vol->motorsAll->motor0,5.0);
    set_power(controle_vol->motorsAll->motor1,5.0);
    set_power(controle_vol->motorsAll->motor2,5.0);
    set_power(controle_vol->motorsAll->motor3,5.0);


    if(controle_vol->verbose){
    	printf("Appuyer sur o quand le calibrage est fini. Merci. \n");
        scanf("%c", &a);
        viderBuffer();
    }else{
    	sleepDuration(5);
    }
    //exit(1);
    return NULL;
}
