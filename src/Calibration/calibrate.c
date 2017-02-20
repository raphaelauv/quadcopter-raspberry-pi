#include "calibrate.h"


void viderBuffer(void){
    int c;
    while((c=getchar()) != EOF && c != '\n');
}

void  Test_puissance(MotorsAll  * motors){

    int high_time=1000;
    while (high_time<=2000) {
        for(int i =0;i<NUMBER_OF_MOTORS;i++){
            set_power(motors->arrayOfMotors[i],high_time);
        }
        high_time+=50;
        sleepDuration(5);
    }
    //exit(1);
}

void calibrate_ESC(MotorsAll * motors,char verbose){

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
