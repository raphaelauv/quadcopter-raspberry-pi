#include "../PWM/PCA9685.h"

int main(int argc, char **argv){


	PCA9685 * pca;
	initPCA9685(&pca,0,0x40);
	PCA9685_setPWMFreq(pca,250);
	PCA9685_setPWM_1(pca,1, 2000);
	PCA9685_setPWM_1(pca,2, 2000);
	PCA9685_setPWM_1(pca,3, 2000);
	PCA9685_setPWM_1(pca,4, 2000);
	pause();


}