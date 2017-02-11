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


void calibrate(MotorsAll * motors,char verbose){

    if(verbose){
    	printf("Calibration Des ESC:\n Pour calibrer les ESCs Brancher la baterie "
    			"APRES AVOIR LANCEE CE PROGRAMME.\nPuis Atender les Deux beep des ESCs et "
    			"appuyer sur o \n Apres cela aller atendre les 3 beep des ESCs, Vos ESC son calibre.\n");
    }

    //PUT 100% of power.
    for(int i =0;i<NUMBER_OF_MOTORS;i++){
    	set_power(motors->arrayOfMotors[i],10.0);
    }

    char a;
    if(verbose){
    	scanf("%c", &a);
    	viderBuffer();
    }else{
    	sleepDuration(5);
    }

    //PUT 0% of power.
    for(int i =0;i<NUMBER_OF_MOTORS;i++){
        set_power(motors->arrayOfMotors[i],5.0);
    }

    if(verbose){
    	printf("Appuyer sur o quand le calibrage est fini. Merci. \n");
        scanf("%c", &a);
        viderBuffer();
    }else{
    	sleepDuration(5);
    }
    //exit(1);
}
