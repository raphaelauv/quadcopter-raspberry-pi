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
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define VAL_LOG_FILE 1
#define VAL_LOG_VERBOSE 2

#define NB_VALUES_TO_LOG 5

int setVerboseOrLog(int argc, char * argv,int min);
void setNoControl(char * NoControl, int argc, char * argv, int min);

void closeLogFile();
void logString(char * str);
int logDataFreq(int * array,int size);
int setDataFrequence(int freq);

/*
#ifdef __cplusplus
}
#endif
*/
#endif /* LOG_H_ */
