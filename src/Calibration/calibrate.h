#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "../motors.h"
#include "../concurrent.h"

void calibrate_ESC(PCA9685 * pcaMotors,char verbose);

#endif /* CALIBRATE_H */
