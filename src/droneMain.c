#include "serv.h"
#include "motors.h"
#include "controldeVol.h"

int main() {


	boolMutex * boolConnectRemote = malloc(sizeof(boolMutex));
	init_boolMutex(boolConnectRemote);

	boolMutex * mutexReadmotors = malloc(sizeof(boolMutex));
	init_boolMutex(mutexReadmotors);

	boolMutex * mutexReadDataControler = malloc(sizeof(boolMutex));
	init_boolMutex(mutexReadDataControler);


	char * adresse = malloc(sizeof(char) * 15);
	getIP(adresse);

	pthread_t threadServer;

	pthread_t threadControlerVOL;

	dataController * dataController = malloc(sizeof(dataController));

	args_SERVER * argServ = malloc(sizeof(args_SERVER));
	argServ->booleanMutex=boolConnectRemote;
	argServ->dataController=dataController;

	struct motor_info * info_m0=malloc(sizeof(struct motor_info));
	struct motor_info * info_m1=malloc(sizeof(struct motor_info));
	struct motor_info * info_m2=malloc(sizeof(struct motor_info));
	struct motor_info * info_m3=malloc(sizeof(struct motor_info));

	motorsAll * motorsAll = malloc(sizeof(motorsAll));
	motorsAll->mutexReadmotors=mutexReadmotors;
	motorsAll->motor0=info_m0;
	motorsAll->motor1=info_m1;
	motorsAll->motor2=info_m2;
	motorsAll->motor3=info_m3;

	args_CONTROLDEVOL * argCONTROLVOL = malloc(sizeof(args_CONTROLDEVOL));
	argCONTROLVOL->mutexReadDataControler=mutexReadDataControler;
	argCONTROLVOL->dataController=dataController;
	argCONTROLVOL->motorsAll=motorsAll;


	pthread_mutex_lock(&boolConnectRemote->mutex);

	if (pthread_create(&threadServer, NULL, thread_TCP_SERVER, argServ)) {
		perror("pthread_create");
		return EXIT_FAILURE;
	}

	pthread_cond_wait(&boolConnectRemote->condition, &boolConnectRemote->mutex);

	pthread_mutex_unlock(&boolConnectRemote->mutex);

	//init_motors( info_m0, info_m1, info_m2, info_m3);//start the 4 threads

	if (pthread_create(&threadControlerVOL, NULL, startCONTROLVOL, argCONTROLVOL)) {
			perror("pthread_create");
			return EXIT_FAILURE;
	}

	if (pthread_join(threadServer, NULL)){
		perror("pthread_join");
		return EXIT_FAILURE;
	}

	free(boolConnectRemote);
	free(argServ);
	return 0;
}
