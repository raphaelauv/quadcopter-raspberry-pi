#include "log.h"


int LOG_verbose_ON=0;
int LOG_file_ON=0;
int LOG_data_ON=0;

char control=1;
char IP_Sound=0;
char doCalibration=0;

int NB_VALUES_TO_LOG=0;

int idFileLog=-1;

int idFileData=-1;

void closeLogFile(){

	if(idFileLog!=-1){
		fsync(idFileLog);
		close(idFileLog);
		idFileLog=-1;
	}

	if(idFileData!=-1){
		fsync(idFileData);
		close(idFileData);
		idFileData=-1;
	}
}

void setFilesName(char * str ,int flag){
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
		sprintf(str,"LOG_%s_(%09ld-%d)",buff, ts.tv_nsec,getpid());
	}else if(flag==FLAG_LOG_DATA){
		sprintf(str,"DATA_%s_(%09ld-%d)",buff, ts.tv_nsec,getpid());
	}

}

int isControl(){
	return control;
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
int tokenAnalyse(int argc , char *argv[],int flag ){

	IP_Sound=0;

	int SHOW_help=0;

	char *argvv;

	if (argc == 1) {
		printf(OPTION_HELP"		to list all options\n");
		return 0;
	}

	int unknow_option;

	for(int i=1; i<argc;i++){
		unknow_option=0;
		argvv=argv[i];

		if(i==1 && flag == FLAG_OPTIONS_CLIENT){
			continue;
			//the first argument of Client exec is the ip Adresse of the drone
		}

		if (strcmp(argvv, OPTION_HELP) == 0) {
			SHOW_help=1;
			break;
		}
		else if (strcmp(argvv, OPTION_CALIBRATION) == 0) {
			if (flag == FLAG_OPTIONS_CLIENT) {
				unknow_option = 1;
			} else {
				printf("calibration ON, ");
				doCalibration = 1;
			}
		}

		else if (strcmp(argvv, OPTION_NO_CONTROL) == 0) {
			/*
			if (flag == FLAG_OPTIONS_CLIENT) {
				unknow_option = 1;
			} else {
			*/
				printf("No controller ON, ");
				control = 0;
			//}
		}
		else if (strcmp(argvv, OPTION_SOUND) == 0) {
			if (flag == FLAG_OPTIONS_CLIENT) {
				unknow_option = 1;
			} else {
				printf("IP Sound ON, ");
				IP_Sound = 1;
			}
		}

		else if (strcmp(argvv, OPTION_VERBOSE) == 0) {
			printf("verbose ON, ");
			LOG_verbose_ON=1;

		} else if (strcmp(argvv, OPTION_LOG) == 0) {

			printf("logFile ON, ");
			LOG_file_ON=1;

			char array[100];
			setFilesName(array,FLAG_LOG_FILE);
			array[99] = '\0';
			printf("\nLOG FILE  NAME : %s\n", array);
			idFileLog = open(array, O_CREAT | O_WRONLY, 0777);

			if (idFileLog == -1) {
				printf("ERROR LOG FILE OPEN\n");
				return -1;
			}


		} else if(strcmp(argvv, OPTION_DATA) == 0) {
			if(flag==FLAG_OPTIONS_CLIENT){
				unknow_option=1;
			}else{
				printf("logData ON, ");
				LOG_data_ON=1;

				char array[100];
				setFilesName(array,FLAG_LOG_DATA);
				array[99] = '\0';
				printf("\nDATA FILE NAME : %s\n", array);
				idFileData = open(array, O_CREAT | O_WRONLY, 0777);
				if (idFileData == -1) {
					printf("ERROR DATA FILE OPEN\n");
					return -1;
				}
			}

		}else{
			unknow_option=1;
		}

		if(unknow_option){
			printf("UNKNOW OPTION : %s\n",argvv);
		}

	}

	if(!SHOW_help){
		printf("\n");
	}
	else{

		printf(OPTION_VERBOSE"	for verbose\n");
		printf(OPTION_LOG"	for log\n");

		if(flag==FLAG_OPTIONS_DRONE){
			printf(OPTION_DATA"	for dataExport\n");
			printf(OPTION_CALIBRATION"	for calibration of the ESC\n");
			printf(OPTION_SOUND"	for SOUND\n");
			printf(OPTION_NO_CONTROL"	for noController\n");
		}
		return -1;
	}


	if( doCalibration && !control){

		printf("ERROR : --cali  and  --noC  | you need CONTROL activate for calibration to simulate the normal behavior\n");
		return -1;
	}
	return 0;
}

int log_frequence=0;
int * arrayData=NULL;


void clean_log_data() {
	if (arrayData != NULL) {
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

	arrayData=(int*)malloc(sizeof(int)*NB_VALUES_TO_LOG);

	for(int i=0;i<NB_VALUES_TO_LOG;i++){
		arrayData[i]=0;
	}
	return 0;
}




void showArrayData(){
	for(int i=0;i<log_frequence;i++){
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
		dprintf(idFileData,"line %s \n",titles);
		return 0;
	}else{
		return -1;
	}

}

int lastDataIndex=0;

int logDataFreq(int * arrayLog,int size){

	if(idFileData==-1){return -1;}

	if(size!=NB_VALUES_TO_LOG){return -1;}

	if(arrayLog==NULL){return -1;}

	for(int i=0;i<size;i++){
		arrayData[i]+=arrayLog[i];
	}

	lastDataIndex++;

	if (lastDataIndex == log_frequence) {
		char arrayStr[SIZE_MAX_LOG];
		int moyenneArray[NB_VALUES_TO_LOG];
		int cmp=0;

		int n = 0;
		int tmp=0;

		for(int i=0;i<NB_VALUES_TO_LOG;i++){
			

			cmp=arrayData[i]/log_frequence;
			moyenneArray[i]=cmp;
			arrayData[i]=0;

			 tmp= sprintf (&(arrayStr[n]), "%d ", moyenneArray[i]);
			 if(tmp>=0){
				 n+=tmp;
			 }else{
				 return -1;
			 }

		}

		//sprintf(arrayStr,"%d %d %d %d %d %d %d %d",moyenneArray[0],moyenneArray[1],moyenneArray[2],moyenneArray[3],moyenneArray[4],moyenneArray[5],moyenneArray[6],moyenneArray[7] );
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
