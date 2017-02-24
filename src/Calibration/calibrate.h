#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "../motors.h"
#include "../concurrent.h"

void calibrate_ESC(MotorsAll2 * motors,char verbose);
void * test_power(void * args);

#endif /* CALIBRATE_H */