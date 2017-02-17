#ifndef LOG_H_
#define LOG_H_

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define VAL_LOG_FILE 1
#define VAL_LOG_VERBOSE 2

void setVerbose(char * verbose, int argc, char * argv,int min);
void setNoControl(char * NoControl, int argc, char * argv, int min);


void logString(char * str);
int logDataFreq(int * array,int size);
int setDataFrequence(int freq);

/*
#ifdef __cplusplus
}
#endif
*/
#endif /* LOG_H_ */
