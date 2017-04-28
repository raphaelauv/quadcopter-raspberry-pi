#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "../motors.h"
//#include "../old/MonoThreadMotor.h"
#include "../concurrent.h"

void calibrate_ESC(MotorsAll * motorsAll3,char verbose);

void test_Power(MotorsAll * motorsAll3);

#endif /* CALIBRATE_H */
