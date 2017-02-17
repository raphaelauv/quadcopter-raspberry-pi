#include "../log.h"

int main(){

	if(setDataFrequence(5)){
		printf("ERROR\n");
	}

	if(setDataFrequence(5)){
		printf("ERROR\n");
	}

	int array[]={5,6,3,5};
	int array2[]={1,2,3,4};
	logDataFreq(array,4);
	logDataFreq(array2,4);
	logDataFreq(array2,4);
	logDataFreq(array2,4);
	logDataFreq(array2,4);
	//logDataFreq(array2,4);
}