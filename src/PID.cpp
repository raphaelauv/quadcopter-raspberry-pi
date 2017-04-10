#include "PID.hpp"


int init_args_PID(args_PID ** argPID,DataController * dataControl,MotorsAll3 * motorsAll3){
    
    *argPID =(args_PID *) malloc(sizeof(args_PID));
    if (*argPID == NULL) {
        logString("MALLOC FAIL : init_args_PID");
        return EXIT_FAILURE;
    }
    
    (*argPID)->dataController = dataControl;
    
#ifdef __arm__
    RTIMU *imu;
    imu = sensorInit();
    
    if(imu==NULL){
        logString("THREAD MAIN : ERROR NEW FAIL RTIMU ->imu");
        return EXIT_FAILURE;
    }else{
        logString("THREAD MAIN : CAPTEUR INIT SUCCES");
        (*argPID)->imu=imu;
    }
#endif
    
    if(initHardwareADC(DEFAULT_CHANNEL_ADCNUM)){
    	logString("initHardwareADC FAIL");
    	return EXIT_FAILURE;
    }

	(*argPID)->motorsAll3=motorsAll3;

    return 0;
}

void clean_args_PID(args_PID * arg) {
    if (arg != NULL) {
        clean_MotorsAll3(arg->motorsAll3);
        clean_DataController(arg->dataController);

#ifdef __arm__
        if( arg->imu !=NULL){
            delete(arg->imu);
            arg->imu=NULL;
        }
#endif
        free(arg);
        arg = NULL;
    }
}




void * thread_PID(void * args);
/**
 * return 0 if Succes
 */
int init_thread_PID(pthread_t * threadPID,void *threadPID_stack_buf,args_PID * argPID){
    
    pthread_attr_t attributs;
    if(init_Attr_Pthread(&attributs,CPU_PRIORITY_PID,CPU_CORE_PID,threadPID_stack_buf)){
        logString("THREAD MAIN : ERROR pthread_attributs PID");
        return -1;
    }
    
    int result=pthread_create(threadPID, &attributs, thread_PID, argPID);
    if (result) {
        logString("THREAD MAIN : ERROR pthread_create PID");
        perror("pthread_create");
    }
    
    pthread_attr_destroy(&attributs);
    
    return result;
}



int absValue(int val){
    if(val<0){
        val*=-1;
    }
    val*=10;
    val+=1000;
    if(val<1000){
        val=1000;
    }
    else if(val>2000){
        val=2000;
    }
    return val;
}

float batteryValue=0;
float batteryTMPVALUE=0;
float batteryVoltage=0;

int applyFiltreBatteryValue(){

	batteryVoltage=hardwareReadADC(DEFAULT_CHANNEL_ADCNUM);

	if(batteryVoltage==-1){
		return -1;
	}
	batteryTMPVALUE=batteryTMPVALUE* 0.92 + (batteryVoltage+65)* 0.09853;
	batteryTMPVALUE=(batteryTMPVALUE * CENVERTION_TO_VOLT) - BATTERY_DECALAGE;

	return 0;

}

void * thread_PID(void * args){
    
	logString("THREAD PID : INITIALISATION");
    args_PID  * controle_vol =(args_PID  *)args;
    DataController * data = controle_vol->dataController;
    PMutex * mutexDataControler =controle_vol->dataController->pmutex;
    RTIMU *imu =(RTIMU *)controle_vol->imu;

    int powerTab[NUMBER_OF_MOTORS];

    int powerController[NUMBER_OF_MOTORS];

    for(int i=0;i<NUMBER_OF_MOTORS;i++){
    	powerController[i]=0;
    }

    int local_period=(1.0/FREQUENCY_PID)*USEC_TO_SEC;
    
    
    //Consigne client
    float client_gaz = MOTOR_LOW_TIME + 50;
    float client_pitch = 0;
    //input PID
    float input_pid_pitch;
    
    //PID
    float output_pid_pitch = 0;
    //erreur des PID
    float pid_erreur_tmp_pitch = 0;
    
    //erreur accu
    float pid_accu_erreur_pitch = 0;
    
    //last erreur
    float pid_last_pitch = 0;
    
    // puissance des 4 moteur. (en microseconde)
    int puissance_motor0 = MOTOR_LOW_TIME;
    int puissance_motor1 = MOTOR_LOW_TIME;
    int puissance_motor2 = MOTOR_LOW_TIME;
    int puissance_motor3 = MOTOR_LOW_TIME;
    
    int log_angle;
    
    RTIMU_DATA imuData;
    
    int nb_values_log=NUMBER_OF_MOTORS+4;

	int logTab[nb_values_log];

    if(setDataFrequence(50,nb_values_log)){
        printf("ERROR setDataFrequence\n");
        //TODO
    }
    
    setDataStringTitle("line Motor1 Motor2 Motor3 Motor4   INPUT  OUTPUT  ANGLE  BATTERY");

    int continuThread=1;

    float gyro_cal[3]={0,0,0};

    /*********************************************************/
    /*				CALIBRATION ACCEL						*/
    /*
    int i;
	#ifdef __arm__
		for (i=0; i<2000; i++) {
			if(imu->IMURead()){
				imuData = imu->getIMUData();
				gyro_cal[0]+=imuData.gyro.x();
				gyro_cal[1]+=imuData.gyro.y();
				gyro_cal[2]+=imuData.gyro.z();
			}
			else{
				printf("trop rapide");
			}
			//usleep(2000);
			nanoSleepSecure(2000 *NSEC_TO_USEC_MULTIPLE);
		}
		gyro_cal[0]/=2000;
		gyro_cal[1]/=2000;
		gyro_cal[2]/=2000;
		printf("Calibration fini\n");
	#endif
    */



    /*********************************************************/
    /*				BATTERY INIT VALUE						*/

    for(int i=0;i<FREQUENCY_PID;i++){
    	if(applyFiltreBatteryValue()){
    		logString("THREAD PID : ERROR BATTERY VALUE");
    		//TODO
    	}

    }
    batteryValue=batteryTMPVALUE;

    /*********************************************************/



    /*********************************************************/
    /*				START PID SECURITY SLEEP				*/
    int numberOfSecondSleep=0;
    char tmpFlag=0;

    if(isCalibration()){
    	numberOfSecondSleep=PID_SLEEP_TIME_SECURITE * PID_SLEEP_VERIF_FREQUENCY;
    	//to skip security sleep
    }else{
    	logString("THREAD PID : SECURITY SLEEP");
    }

    int nanoSleepTimeInterval = NSEC_TO_SEC / PID_SLEEP_VERIF_FREQUENCY;

    while(numberOfSecondSleep<PID_SLEEP_TIME_SECURITE * PID_SLEEP_VERIF_FREQUENCY){


    	numberOfSecondSleep++;
    	pthread_mutex_lock(&(mutexDataControler->mutex));
    	tmpFlag=data->flag;
    	pthread_mutex_unlock(&(mutexDataControler->mutex));
		if (tmpFlag == 0) {
			continuThread = 0;
			break;
		}
		if (is_Motor_Stop(controle_vol->motorsAll3)) {
			continuThread = 0;
			continue;
		}

    	else{
			nanoSleepSecure(nanoSleepTimeInterval);
		}
    }
    /*********************************************************/


    struct timeval tv;
    int iterRemote=0;
    int iterBattery=0;
    int timeUsecStart=0;
    int timeUsecEnd=0;
    int timeBetween=0;

    int readSensorSucces=0;

    char arrayLog[SIZE_MAX_LOG];
    if(continuThread){
    	logString("THREAD PID : START");
    }
    while (continuThread) {

        gettimeofday(&tv, NULL);
        timeUsecStart= (int)tv.tv_sec * USEC_TO_SEC + (int)tv.tv_usec;

        if(is_Motor_Stop(controle_vol->motorsAll3)){
        	continuThread = 0;
        	continue;
        }
        
        /*********************************************************/
        /*					CODE FOR GET REMOTE					*/
        iterRemote++;
        if(iterRemote>FREQUENCY_PID/FREQUENCY_CONTROLLER){
        	iterRemote=0;
            pthread_mutex_lock(&(mutexDataControler->mutex));
            if (data->flag== 0) {
                pthread_mutex_unlock(&(mutexDataControler->mutex));
                break;//TODO
            }else{
            	mutexDataControler->var = 0;
				powerController[0] = data->axe_Rotation;
				powerController[1] = data->axe_UpDown;
				powerController[2] = data->axe_LeftRight;
				powerController[3] = data->axe_FrontBack;
				pthread_mutex_unlock(&(mutexDataControler->mutex));

				/*
				powerTab[0] =absValue(powerController[0]);
				powerTab[1] =absValue(powerController[1]);
				powerTab[2] =absValue(powerController[2]);
				powerTab[3] =absValue(powerController[3]);

				//printf("VALUES %d %d %d %d\n", powerTab[0],powerTab[1],powerTab[2],powerTab[3]);
				set_power2(controle_vol->motorsAll2,powerTab);
				logDataFreq(powerTab,NUMBER_OF_MOTORS);
				*/

            }
            
        }

        /*********************************************************/
		/*					CODE BATTERY				*/
		iterBattery++;
		if(iterBattery>FREQUENCY_PID){
			batteryValue=batteryTMPVALUE;
			iterBattery=0;
		}
		if(applyFiltreBatteryValue()){
			logString("THREAD PID : ERROR BATTERY VALUE");
			//TODO
		}

		#ifdef __arm__
        if(imu->IMURead()){
        	readSensorSucces=1;
        }else{
        	readSensorSucces=0;
        }
		#endif

        if(readSensorSucces){
			#ifdef __arm__
            imuData = imu->getIMUData();
			#endif
            //input PID
            input_pid_pitch=(input_pid_pitch*0.7) + ((imuData.gyro.y()-gyro_cal[1])*(180/M_PI)*0.3);
            
            if(powerController[1]>=0){
                client_gaz=(powerController[1]*7)+1100;
            }
            else{
                client_gaz=1100;
            }
            
            client_pitch=powerController[2] * PID_ANGLE_PRECISION_MULTIPLE;

            log_angle=imuData.fusionPose.y() * RTMATH_RAD_TO_DEGREE;


            client_pitch-= log_angle * PID_ANGLE_MULTIPLE;

            client_pitch/=3;

            //calcule pitch PID
            pid_erreur_tmp_pitch=input_pid_pitch-client_pitch;
            pid_accu_erreur_pitch+=PID_GAIN_I_PITCH*pid_erreur_tmp_pitch;
            if (pid_accu_erreur_pitch>PID_MAX_PITCH) {
                pid_accu_erreur_pitch=PID_MAX_PITCH;
            }
            else if (pid_accu_erreur_pitch < -PID_MAX_PITCH){
                pid_accu_erreur_pitch=-PID_MAX_PITCH;
            }
            
            output_pid_pitch=PID_GAIN_P_PITCH*pid_erreur_tmp_pitch+pid_accu_erreur_pitch+PID_GAIN_D_PITCH*(pid_erreur_tmp_pitch-pid_last_pitch);
            
            if (output_pid_pitch>PID_MAX_PITCH) {
                output_pid_pitch=PID_MAX_PITCH;
            }
            else if (output_pid_pitch < -PID_MAX_PITCH){
                output_pid_pitch=-PID_MAX_PITCH;
            }
            pid_last_pitch=pid_erreur_tmp_pitch;
            
            
            puissance_motor0=client_gaz + output_pid_pitch;
            puissance_motor1=client_gaz - output_pid_pitch;
            
            //Pour jamais mettre a l'arret les moteur.
            if(puissance_motor0<MOTOR_MIN_ROTATE_TIME) puissance_motor0=MOTOR_MIN_ROTATE_TIME;
            if(puissance_motor1<MOTOR_MIN_ROTATE_TIME) puissance_motor1=MOTOR_MIN_ROTATE_TIME;
            if(puissance_motor2<MOTOR_MIN_ROTATE_TIME) puissance_motor2=MOTOR_MIN_ROTATE_TIME;
            if(puissance_motor3<MOTOR_MIN_ROTATE_TIME) puissance_motor3=MOTOR_MIN_ROTATE_TIME;
            
            //puissance max=MOTOR_HIGH_TIME donc il faut pas depasser.
            if(puissance_motor0>MOTOR_HIGH_TIME) puissance_motor0=MOTOR_HIGH_TIME;
            if(puissance_motor1>MOTOR_HIGH_TIME) puissance_motor1=MOTOR_HIGH_TIME;
            if(puissance_motor2>MOTOR_HIGH_TIME) puissance_motor2=MOTOR_HIGH_TIME;
            if(puissance_motor3>MOTOR_HIGH_TIME) puissance_motor3=MOTOR_HIGH_TIME;
            
            powerTab[0] = puissance_motor0;
            powerTab[1] = puissance_motor1;
            powerTab[2] = 1000;
            powerTab[3] = 1000;
            //powerTab[2] = puissance_motor2;
            //powerTab[3] = puissance_motor3;

            logTab[0]=powerTab[0];
            logTab[1]=powerTab[1];
            logTab[2]=powerTab[2];
            logTab[3]=powerTab[3];
            logTab[4]=(int)input_pid_pitch;
            logTab[5]=(int)output_pid_pitch;
            logTab[6]=(int)log_angle;
            logTab[7]=(int)(batteryValue*100);//TODO

            if(isCalibration()){
            	//nothing to apply because we are in a calibrate mode execution
            }else{
            	set_power3(controle_vol->motorsAll3,powerTab);
            }
            logDataFreq(logTab,nb_values_log);

        }

        
        /*********************************************************/
        /*			CODE FOR SLEEP PID FREQUENCY				*/
        
        gettimeofday(&tv, NULL);
        timeUsecEnd= (int)tv.tv_sec * USEC_TO_SEC + (int)tv.tv_usec;
        timeBetween=timeUsecEnd - timeUsecStart;
        
        if(timeBetween<0){
        }
        else if(timeBetween>=local_period){
        	sprintf(arrayLog,"THREAD PID : TIME : %d\n",local_period-timeBetween);
        	logString(arrayLog);
        }else{
        	nanoSleepSecure( (local_period-timeBetween) * NSEC_TO_USEC_MULTIPLE);
        }
    }
    set_Motor_Stop(controle_vol->motorsAll3);
    logString("THREAD PID : END");
    return NULL;
}
