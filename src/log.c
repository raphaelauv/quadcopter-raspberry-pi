#include "log.h"

//int verbose_or_log=0;

int LOG_verbose_ON=0;
int LOG_file_ON=0;
int LOG_data_ON=0;


int NB_VALUES_TO_LOG=0;

int idFileLog=-1;

int idFileData=-1;

void closeLogFile(){
	if(idFileLog!=-1){
		close(idFileLog);
		idFileLog=-1;
		//verbose_or_log=FLAG_VAL_VERBOSE;
	}
}

void setLogFileName(char * str ,int flag){
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
	if(flag==FLAG_LOG_FILE){
		sprintf(str,"LOG_%s.%09ld_%d",buff, ts.tv_nsec,getpid());
	}else if(flag==FLAG_LOG_DATA){
		sprintf(str,"DATA_%s.%09ld_%d",buff, ts.tv_nsec,getpid());
	}

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
	return LOG_verbose_ON;
}

/*
 * Return -1 if FAIL , else return 0 in SUCCES
 */
int tokenAnalyse(int argc , char *argv[] ){

	noControl=0;
	IP_Sound=0;

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
		else if (strcmp(argvv, "--cali") == 0) {
			printf("calibration ON, ");
			doCalibration = 1;
		}

		else if (strcmp(argvv, "--noC") == 0) {
			printf("No controller ON, ");
			noControl = 1;
		}
		else if (strcmp(argvv, "--s") == 0) {
			printf("IP Sound ON, ");
			IP_Sound = 1;
		}

		else if (strcmp(argvv, "--verb") == 0) {
			printf("verbose ON, ");
			LOG_verbose_ON=1;
			//verbose_or_log = FLAG_VAL_VERBOSE;

		} else if (strcmp(argvv, "--log") == 0) {

			printf("logFile ON, ");
			LOG_file_ON=1;
			//verbose_or_log = FLAG_VAL_FILE;

			char array[100];
			setLogFileName(array,FLAG_LOG_FILE);
			array[99] = '\0';
			printf("\nLOG FILE  NAME : %s\n", array);
			idFileLog = open(array, O_CREAT | O_WRONLY, 0777);

			if (idFileLog == -1) {
				printf("ERROR LOG FILE OPEN\n");
				return -1;
			}


		} else if(strcmp(argvv, "--data") == 0) {
			printf("logData ON, ");
			LOG_data_ON=1;

			char array[100];
			setLogFileName(array,FLAG_LOG_DATA);
			array[99] = '\0';
			printf("\nDATA FILE NAME : %s\n", array);
			idFileData = open(array, O_CREAT | O_WRONLY, 0777);
			if (idFileData == -1) {
				printf("ERROR DATA FILE OPEN\n");
				return -1;
			}

		}else{
			printf("UNKNOW OPTION : %s\n",argvv);
		}

	}

	if(!SHOW_help){
		printf("\n");
	}
	else{
		printf("--verb	for verbose\n");
		printf("--log	for log\n");
		printf("--data	for dataExport\n");
		printf("--cali	for calibration of the ESC\n");
		printf("--s		for SOUND\n");
		printf("--noC	for noController\n");
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



int cmpLogData=0;
void logDataString(char * str){
	if(str==NULL || idFileData==-1){return;}

	dprintf(idFileData,"%d %s \n",cmpLogData,str);
	cmpLogData++;

}

int titleNeverGiveYet=1;
int setDataStringTitle(char * titles){
	if(titleNeverGiveYet==1 && cmpLogData==0){
		dprintf(idFileData,"%s \n",titles);
		return 0;
	}else{
		return -1;
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
		char arrayStr[SIZE_MAX_LOG];
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
		//sprintf(arrayStr,"DATAFREQ : FREQ=%d;%d;%d;%d;%d;%d;%d;%d",log_frequence,moyenneArray[0],moyenneArray[1],moyenneArray[2],moyenneArray[3],moyenneArray[4],moyenneArray[5],moyenneArray[6] );
		//logString(arrayStr);
		sprintf(arrayStr,"%d %d %d %d %d %d %d",moyenneArray[0],moyenneArray[1],moyenneArray[2],moyenneArray[3],moyenneArray[4],moyenneArray[5],moyenneArray[6] );
		logDataString(arrayStr);
		lastDataIndex = 0;
	}

	return 0;
}


void logString(char * str){

	if(str==NULL){return;}

	if(LOG_file_ON){
		struct timespec ts;
		timespec_get(&ts, TIME_UTC);
		char buff[100];
		strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
		dprintf(idFileLog,"[%s.%09ld] : %s ;\n", buff, ts.tv_nsec,str);

	}
	if(LOG_verbose_ON){
		printf("%s\n",str);
	}
}
