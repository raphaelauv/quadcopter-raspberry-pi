#include "log.h"

int verbose_or_log=0;

void setVerbose(char * verbose, int argc, char * argv,int min) {
	if (argc > min) {
		if (strcmp(argv, "--verbose") == 0) {
			printf("verbose MODE select\n");
			*verbose = 1;
			verbose_or_log=VAL_LOG_VERBOSE;
		}
		else if (strcmp(argv, "--log") == 0) {
			printf("log MODE select\n");
			*verbose = 1;
			verbose_or_log = VAL_LOG_FILE;
		}
	} else {
		printf("add    --verbose   for verbose mode\n");
	}
}

void setNoControl(char * NoControl, int argc, char * argv, int min) {
	if (argc > min) {
		if (strcmp(argv, "--noControl") == 0) {
			printf("No controller MODE select\n");
			*NoControl = 1;
		}
	} else {
		printf("add    --noControl  for noController mode\n");
	}
}

int log_frequence=0;

int ** arrayData=NULL;

void clean_log_data() {
	if (arrayData != NULL) {
		for (int i = 0; i < log_frequence; i++) {
			if (arrayData[i] == NULL) {
				free(arrayData[i]);
			}
		}
		free(arrayData);
	}
}

int setDataFrequence(int freq){
	if (freq<0){return -1;}
	else{
		log_frequence=freq;
	}
	clean_log_data();
	arrayData=(int**)malloc(sizeof(int*)*log_frequence);
	if(arrayData==NULL){
			return -1;
	}
	for(int i=0;i<log_frequence;i++){
		arrayData[i]=(int*)malloc(sizeof(int)*5);
		if(arrayData[i]==NULL){
			return -1;
		}
	}

	return 0;
}




void showArrayData(){
	for(int i=0;i<log_frequence;i++){
		for(int j=0;j<5;j++){
			printf("%d ",arrayData[i][j]);
		}
		printf("\n");
	}

}

int lastDataIndex=0;
int logDataFreq(int * array,int size){


	if(array==NULL){return -1;}

	for(int i=0;i<size;i++){
		arrayData[lastDataIndex][i]=array[i];
	}

	if (lastDataIndex + 1 == log_frequence) {
		showArrayData();
		lastDataIndex = 0;
	}else{
		lastDataIndex++;
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

	if(verbose_or_log==VAL_LOG_FILE){
		struct timespec ts;
		timespec_get(&ts, TIME_UTC);
		char buff[100];
		strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
		printf("[%s.%09ld] : %s", buff, ts.tv_nsec,str);

	}else if(verbose_or_log==VAL_LOG_VERBOSE){
		printf("%s",str);
	}
}
