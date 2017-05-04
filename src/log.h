#ifndef LOG_H_
#define LOG_H_

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define OPTION_PREFIX "--"
#define OPTION_CALIBRATION OPTION_PREFIX"cali"
#define OPTION_TESTPOWER OPTION_PREFIX"testpower"
#define OPTION_NO_CONTROL OPTION_PREFIX"noC"
#define OPTION_VERBOSE OPTION_PREFIX"verb"
#define OPTION_LOG OPTION_PREFIX"log"
#define OPTION_DATA OPTION_PREFIX"data"
#define OPTION_SOUND OPTION_PREFIX"s"
#define OPTION_HELP OPTION_PREFIX"help"


#define FLAG_OPTIONS_DRONE 1
#define FLAG_OPTIONS_CLIENT 2

#define ABSOLUTE_REFERENCE_FILE "~"
#ifdef __arm__
#define ABSOLUTE_REFERENCE_FILE "/home/pi"
#endif

#define FLAG_LOG_FILE 1
#define FLAG_LOG_DATA 2

#define SIZE_MAX_LOG 400

#define FLAG_VAL_FILE 1
#define FLAG_VAL_VERBOSE 2

//int setVerboseOrLog(int argc, char * argv,int min);
//void setNoControl(char * NoControl, int argc, char * argv, int min);

int tokenAnalyse(int argc , char *argv[],int flag );

int isTestpower();
int isCalibration();
int isControl();
int isIpSound();
int isVerbose();

void closeLogFile();
void logString(char * str);

int logDataFreq(int * array,int size);
int setDataStringTitle(char * titles);

int setDataFrequence(int freq,int nb_values);


/*
#ifdef __cplusplus
}
#endif
*/
#endif /* LOG_H_ */
