#include "log.h"

int verbose_or_log=0;
int idFile=-1;

void closeLogFile(){
	if(idFile!=-1){
		close(idFile);
		idFile=-1;
		verbose_or_log=VAL_LOG_VERBOSE;
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
		tmp++;
	}
	sprintf(str,"LOG_%s.%09ld_%d",buff, ts.tv_nsec,getpid());
}


/*
 * Return -1 if fail , 0 if Succes
 */
int setVerboseOrLog(int argc, char * argv,int min) {
	if (argc > min) {
		if (strcmp(argv, "--verbose") == 0) {
			printf("verbose MODE select\n");
			verbose_or_log=VAL_LOG_VERBOSE;
		}
		else if (strcmp(argv, "--log") == 0) {
			printf("log MODE select\n");
			verbose_or_log = VAL_LOG_FILE;

			char array[100];
			setLogFileName(array);
			array[99]='\0';
			printf("FILE NAME : %s\n",array);
			idFile=open(array,O_CREAT|O_WRONLY,0777);

			if(idFile==-1){
				printf("ERROR FILE OPEN\n");
				return -1;
			}
		}
	} else {
		printf("add		--verbose	for verbose	mode\n");
		printf("add		--log		for log 	mode\n");
	}

	return 0;
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
		dprintf(idFile,"[%s.%09ld] : %s ;\n", buff, ts.tv_nsec,str);

	}else if(verbose_or_log==VAL_LOG_VERBOSE){
		printf("%s\n",str);
	}
}
