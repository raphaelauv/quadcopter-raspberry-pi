#include "log.h"

int verbose_or_log=0;
int NB_VALUES_TO_LOG=0;

int idFile=-1;

void closeLogFile(){
	if(idFile!=-1){
		close(idFile);
		idFile=-1;
		verbose_or_log=VAL_VERBOSE;
	}
}

void setLogFileName(char * str){
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	char buff[100];
	strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
	int tmp=0;
	while(buff[tmp]!='\0'){
		if(buff[tmp]=='/'){
			buff[tmp]='-';
		}
		else if(buff[tmp]==' '){
			buff[tmp]='-';
		}
		tmp++;
	}
	sprintf(str,"LOG_%s.%09ld_%d",buff, ts.tv_nsec,getpid());
}

int isNoControl(){
	return noControl;
}

int isIpSound(){
	return IP_Sound;
}

int isCalibration(){
	return doCalibration;
}

int isVerbose(){
	return verbose_or_log==VAL_VERBOSE;
}

/*
 * Return -1 if FAIL , else return 0 in SUCCES
 */
int tokenAnalyse(int argc , char *argv[] ){

	noControl=0;
	IP_Sound=0;
	int SHOW_LogOrVerbose=1;
	int SHOW_noControl=1;
	int SHOW_doCalibration=1;
	int SHOW_IpSound=1;
	int SHOW_help=0;
	char *argvv;

	if (argc == 1) {
		printf("add		--help		to list all options\n");
		return 0;
	}

	for(int i=1; i<argc;i++){

		argvv=argv[i];

		if (strcmp(argvv, "--help") == 0) {
			SHOW_help=1;
			break;
		}
		else if (strcmp(argvv, "--calibration") == 0) {
			printf("calibration ON, ");
			doCalibration = 1;
			SHOW_doCalibration = 0;
		}

		else if (strcmp(argvv, "--noControl") == 0) {
			printf("No controller ON, ");
			noControl = 1;
			SHOW_noControl=0;
		}
		else if (strcmp(argvv, "--ipSound") == 0) {
			printf("IP Sound ON, ");
			IP_Sound = 1;
			SHOW_IpSound = 0;
		}

		else if (strcmp(argvv, "--verbose") == 0) {
			if (SHOW_LogOrVerbose == 0) {
				printf("\nVERBOSE OR LOG NOT BOTH\n");
			} else {
				printf("verbose ON, ");
				verbose_or_log = VAL_VERBOSE;
				SHOW_LogOrVerbose = 0;
			}

		} else if (strcmp(argvv, "--log") == 0) {
			if (SHOW_LogOrVerbose == 0) {
				printf("\nVERBOSE OR LOG NOT BOTH\n");
			} else {
				printf("log ON, ");
				verbose_or_log = VAL_FILE;

				char array[100];
				setLogFileName(array);
				array[99] = '\0';
				printf("\nFILE NAME : %s\n", array);
				idFile = open(array, O_CREAT | O_WRONLY, 0777);

				if (idFile == -1) {
					printf("ERROR FILE OPEN\n");
					return -1;
				}
				SHOW_LogOrVerbose = 0;

			}
		}

	}

	if(!SHOW_help){
		printf("\n");
	}
	if (SHOW_LogOrVerbose) {
		printf("--verbose	for verbose\n");
		printf("--log		for log\n");
	}
	if (SHOW_noControl){
		printf("--noControl	for noController\n");
	}
	if (SHOW_IpSound) {
		printf("--ipSound	for IP SOUND\n");
	}
	if (SHOW_doCalibration) {
		printf("--calibration	for calibration of the ESC\n");
	}

	if(SHOW_help){
		return -1;
	}
	return 0;
}

int log_frequence=0;

//int ** arrayData=NULL;
int * arrayData=NULL;


void clean_log_data() {
	if (arrayData != NULL) {
		/*
		for (int i = 0; i < log_frequence; i++) {
			if (arrayData[i] == NULL) {
				free(arrayData[i]);
			}
		}
		*/
		free(arrayData);
	}
}

int setDataFrequence(int freq,int nb_values){
	if (freq<0 || nb_values<0){return -1;}
	else{
		log_frequence=freq;
		NB_VALUES_TO_LOG=nb_values;
	}

	clean_log_data();
	/*

	arrayData=(int**)malloc(sizeof(int*)*log_frequence);
	if(arrayData==NULL){
		return -1;
	}
	for(int i=0;i<log_frequence;i++){
		arrayData[i]=(int*)malloc(sizeof(int)*NB_VALUES_TO_LOG);
		if(arrayData[i]==NULL){
			return -1;
		}
	}
	*/

	arrayData=(int*)malloc(sizeof(int)*NB_VALUES_TO_LOG);

	for(int i=0;i<NB_VALUES_TO_LOG;i++){
		arrayData[i]=0;
	}
	return 0;
}




void showArrayData(){
	for(int i=0;i<log_frequence;i++){
		/*
		for(int j=0;j<NB_VALUES_TO_LOG;j++){
			printf("%d ",arrayData[i][j]);
		}
		*/

		printf("%d ",arrayData[i]);
		printf("\n");
	}

}


void fillStrLog(int size, ...){

	va_list va;
	va_start (va, size);

	for (int i = 0; i < size; i++) {
		int c = va_arg(va, int);
	}

}

int lastDataIndex=0;

int logDataFreq(int * arrayLog,int size){

	if(size>NB_VALUES_TO_LOG){return -1;}

	if(arrayLog==NULL){return -1;}

	for(int i=0;i<size;i++){
		//arrayData[lastDataIndex][i]=arrayLog[i];
		arrayData[i]+=arrayLog[i];
	}

	lastDataIndex++;

	if (lastDataIndex == log_frequence) {
		char arrayStr[400];
		int moyenneArray[NB_VALUES_TO_LOG];
		int cmp=0;
		for(int i=0;i<NB_VALUES_TO_LOG;i++){
			
			/*
			cmp=0;
			for(int j=0;j<log_frequence;j++){
				cmp+=arrayData[j][i];
			}
			
			cmp=cmp/log_frequence;
			*/
			cmp=arrayData[i]/log_frequence;
			moyenneArray[i]=cmp;
			arrayData[i]=0;
		}
		//showArrayData();
		sprintf(arrayStr,"DATAFREQ : FREQ=%d;%d;%d;%d;%d;%d;%d;%d",log_frequence,moyenneArray[0],moyenneArray[1],moyenneArray[2],moyenneArray[3],moyenneArray[4],moyenneArray[5],moyenneArray[6] );
		logString(arrayStr);
		lastDataIndex = 0;
	}

	return 0;
}


void logResumeOfData(){
	//char data[600];

	//logString(data);

	showArrayData();
}

void logString(char * str){

	if(str==NULL){return;}

	if(verbose_or_log==VAL_FILE){
		struct timespec ts;
		timespec_get(&ts, TIME_UTC);
		char buff[100];
		strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
		dprintf(idFile,"[%s.%09ld] : %s ;\n", buff, ts.tv_nsec,str);

	}else if(verbose_or_log==VAL_VERBOSE){
		printf("%s\n",str);
	}
}
