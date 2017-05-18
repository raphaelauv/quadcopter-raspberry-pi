#ifndef CONCURRENT_H_
#define CONCURRENT_H_

/*
#ifdef __cplusplus
#define NULL nullptr
#endif
*/

//#define _GNU_SOURCE

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/signal.h>
#include <unistd.h>


#include "log.h"

#define ERROR(a,str) if (a<0) {perror(str); exit(1);}

void init_mask(void (*functionPtr)(int));


#define FREQUENCY_CONTROLLER 10.0
#define FREQUENCY_PID 250.0
#define FREQUENCY_MOTOR 50.0



#define MSEC_TO_SEC_MULTIPLE 1000 //millisecond
#define USEC_TO_MSEC_MULTIPLE 1000
#define NSEC_TO_USEC_MULTIPLE 1000
#define USEC_TO_SEC MSEC_TO_SEC_MULTIPLE * 1000 //microsecond
#define NSEC_TO_SEC USEC_TO_SEC * 1000 //nanosecond
#define MSEC_TO_NSEC 1000 * 1000
#define SEC_TO_NSEC 1000 * 1000 * 1000

#define CPU_CORE_PID 1
#define CPU_CORE_MOTOR 0

#define CPU_PRIORITY_PID 99
#define CPU_PRIORITY_MOTOR 99



//Custom Mutex
typedef struct PMutex {
	int var;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
} PMutex;

int init_PMutex(PMutex * arg);
void clean_PMutex(PMutex * arg);
void barriereWait(PMutex * Barrier,int size);

typedef struct DataController{

	//FLAG 0-> ARRET | 1 -> PAUSE | 2 -> NORMAL | 3 -> ...
	char flag;
	float axe_Rotation;
	float axe_UpDown;

	float axe_LeftRight;
	float axe_FrontBack;

	PMutex * pmutex;

}DataController;

typedef struct PID_INFO{
	float battery;
	PMutex * pmutex;
}PID_INFO;


void clean_PID_INFO(PID_INFO * arg);

void clean_DataController(DataController * arg);

int init_Attr_Pthread(pthread_attr_t *attributs, int priority,int id_cpu,void *stack_buf);


#define SOUND_PLAYER "mpg123 -q "
#ifdef __arm__
#define SOUND_FOLDER "/home/pi/drone/Lib/sound/"
#else
#define SOUND_FOLDER " ~/drone/Lib/sound/"
#endif

#define SOUND_COMMAND SOUND_PLAYER SOUND_FOLDER

#define SOUND_PAUSE_TIME 400000




//sleep nano seconde , if fail return -1 else return 0
inline int clockNanoSleepSecure(long nano) {

	struct timespec tim, tim2;

	tim.tv_sec = 0;
	tim.tv_nsec = nano;

	/*
	if(nano>=999999999){
		logString("ERROR NANOSLEEP TOO BIG");
	}

	if (nano < 0) {
		logString("EROR NANOSLEEP TOO LOW");
	}
	 */
	int i=0;

	int result = 1;
	//result = nanosleep(&tim, &tim2);
	result= clock_nanosleep(CLOCK_MONOTONIC,0,&tim,&tim2);

	if(result){
		logString("NANOSLEEP ERROR");
	}

	/* TODO a tester
	i++;
	while (result != 0) {

		if (errno == EINTR) {
			logString("NANOSLEEP ERROR EINTR");
			if(i%2==0){
				result=nanosleep(&tim,&tim2);
			}else{
				result=nanosleep(&tim2,&tim);
			}
			i++;
		}
		else if (errno == EINVAL){
			logString("NANOSLEEP ERROR EINVAL");
			return -1;
		}
		else{
			logString("NANOSLEEP ERROR UNKNOW");
			return -1;
		}
	}

	if(i>1){
		logString("NANOSLEEP CORRECTED");
	}

	*/

	return 0;

	//*/
}


/*
#ifdef __cplusplus
}
#endif
*/
#endif /* CONCURRENT_H_ */
