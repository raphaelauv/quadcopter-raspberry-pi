#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "../motors.h"
//#include "../old/MonoThreadMotor.h"
#include "../concurrent.h"

void calibrate_ESC(MotorsAll * motorsAll3,char verbose);

#endif /* CALIBRATE_H */
