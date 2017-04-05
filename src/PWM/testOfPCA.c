#include "PCA9685.h"

int main(int argc, char **argv){


	PCA9685 * pca;
	initPCA9685(&pca,0,0x40);
	custom_setPWMFreq(pca,250);
	custom_setPWM_1(pca,1, 2000);
	custom_setPWM_1(pca,2, 2000);
	custom_setPWM_1(pca,3, 2000);
	custom_setPWM_1(pca,4, 2000);
	pause();


}
