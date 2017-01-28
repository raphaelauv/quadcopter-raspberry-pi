//
//  Test_puissance.c
//  Drone
//
//  Created by andres quiroz on 27/01/2017.
//  Copyright © 2017 andres quiroz. All rights reserved.
//

#include "Test_puissance.h"

void  Test_puissance(MotorsAll  * motors){

    int high_time=1000;
    while (high_time<=2000) {
        set_power(motors->motor0,high_time);
        set_power(motors->motor1,high_time);
        set_power(motors->motor2,high_time);
        set_power(motors->motor3,high_time);
        high_time+=50;
        sleepDuration(5);
    }
    //exit(1);
}
