//
//  Test_puissance.cpp
//  Drone
//
//  Created by andres quiroz on 27/01/2017.
//  Copyright © 2017 andres quiroz. All rights reserved.
//

#include "Test_puissance.hpp"

void * Test_puissance(void * args){
    args_CONTROLDEVOL  * controle_vol =(args_CONTROLDEVOL *)args;
    int high_time=1000;
    while (high_time<=2000) {
        set_power(controle_vol->motorsAll->motor0,high_time);
        set_power(controle_vol->motorsAll->motor1,high_time);
        set_power(controle_vol->motorsAll->motor2,high_time);
        set_power(controle_vol->motorsAll->motor3,high_time);
        high_time+=50;
        sleep(5);
    }
    exit(1);
    return NULL;
}