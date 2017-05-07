#include "PID.hpp"


int init_args_PID(args_PID ** argPID){
    
    *argPID =(args_PID *) malloc(sizeof(args_PID));
    if (*argPID == NULL) {
        logString("MALLOC FAIL : init_args_PID");
        return EXIT_FAILURE;
    }
    
    
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


    return 0;
}

void clean_args_PID(args_PID * arg) {
    if (arg != NULL) {
        clean_MotorsAll(arg->motorsAll3);
        clean_DataController(arg->dataController);

#ifdef __arm__
        if( arg->imu !=NULL){
            delete(arg->imu);
            arg->imu=NULL;
        }
#endif
        free(arg);
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

int applyFiltreBatteryValue(){

	int batteryVoltage=hardwareReadADC(DEFAULT_CHANNEL_ADCNUM);
	if(batteryVoltage==-1){
		return -1;
	}

	//PUT VALUES IN MACRO
	batteryTMPVALUE=batteryTMPVALUE* 0.92 + (batteryVoltage+60)* 0.09509;
	//	batteryTMPVALUE=(batteryTMPVALUE * CONVERTION_TO_VOLT);

	batteryValue=batteryTMPVALUE;

	return 0;

}

void * thread_PID(void * args){
	int continuThread=1;


	logString("THREAD PID : INITIALISATION");

    args_PID  * controle_vol =(args_PID  *)args;
    DataController * data = controle_vol->dataController;
    PMutex * mutexDataControler =controle_vol->dataController->pmutex;
    PID_INFO * pidInfo =controle_vol->pidInfo;

	#ifdef __arm__
    RTIMU *imu =(RTIMU *)controle_vol->imu;
	#endif

    long local_period=(1.0/FREQUENCY_PID) *SEC_TO_NSEC;

    
    //Consigne client
    float client_gaz = MOTOR_LOW_TIME + 50;
    float client_pitch = 0;
    float client_roll=0;
    float client_yaw=0;
    
    //input PID
    float input_pid_pitch;
    float input_pid_roll;
    float input_pid_yaw;
    //PID
    float output_pid_pitch = 0;
    float output_pid_roll = 0;
    float output_pid_yaw = 0;
    //erreur des PID
    float pid_erreur_tmp_pitch = 0;
    float pid_erreur_tmp_roll = 0;
    float pid_erreur_tmp_yaw = 0;
    //erreur accu
    float pid_accu_erreur_pitch = 0;
    float pid_accu_erreur_roll = 0;
    float pid_accu_erreur_yaw = 0;
    //last erreur
    float pid_last_pitch = 0;
    float pid_last_roll = 0;
    float pid_last_yaw = 0;
    
    // puissance des 4 moteur. (en microseconde)
    int puissance_motor0 = MOTOR_LOW_TIME;
    int puissance_motor1 = MOTOR_LOW_TIME;
    int puissance_motor2 = MOTOR_LOW_TIME;
    int puissance_motor3 = MOTOR_LOW_TIME;
    int powerTab[NUMBER_OF_MOTORS]={0};
    int powerController[NUMBER_OF_MOTORS]={0};
    
    
    RTIMU_DATA imuData;
    float gyro_cal[3]={0,0,0};

    
    int nb_values_log=NUMBER_OF_MOTORS+2+(3*3)+1;//( motors -> 4 ) + 2 angles ( (pitch roll yaw) * 3 ) + BatteryLevel
	int logTab[nb_values_log];
	int log_angle_pitch;
	int log_angle_roll;
	char arrayLog[SIZE_MAX_LOG];



    struct timespec t0, t1 ,tim;
    int modeCalibration=isCalibration();

    int iterRemote=0;
    int iterBattery=0;
    int iterPrintPower=0;
    int iterBatteryPrint=0;
    int readSensorSucces=0;


    /********VIBRATION******/
    float acc_total_vector[VIBRATION_Moving_average]={0};
    float acc_av_vector;
    float vibration_total_result;
    float acc_x;
    float acc_y;
    float acc_z;
    int iterAccelPrint=0;
    int iterVibration;
    int testvibration=isTestVibration();

    float lastVibration;
    /**********************/

    int iterSecuritySleep=0;
    int nanoSleepTimeIntervalOfSecuritySleep = NSEC_TO_SEC / PID_SLEEP_VERIF_FREQUENCY;
    char tmpFlagRemoteMSG=0;


    if(setDataFrequence(50,nb_values_log)){
    	logString("THREAD PID : ERROR setDataFrequence");
    	goto endPid;
    }
    
    if (setDataStringTitle(
			"Motor1 Motor2 Motor3 Motor4 anglePITCH angleROLL clientPITCH clientROLL clientYAW inputPidPITCH inputPidROLL inputPidYAW outputPidPITCH outputPidROLL outputPidYAW Battery")) {
    	logString("THREAD PID : ERROR setDataStringTitle");
    	goto endPid;
	}





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
    		goto endPid;
    	}
    }

    /*********************************************************/



    /*********************************************************/
    /*				START PID SECURITY SLEEP				*/

    if(isCalibration()){
    	iterSecuritySleep=PID_SLEEP_TIME_SECURITE * PID_SLEEP_VERIF_FREQUENCY;
    	//to skip security sleep
    }else{
    	logString("THREAD PID : SECURITY SLEEP");
    }
    while(iterSecuritySleep<PID_SLEEP_TIME_SECURITE * PID_SLEEP_VERIF_FREQUENCY){

    	iterSecuritySleep++;
    	pthread_mutex_lock(&(mutexDataControler->mutex));
    	tmpFlagRemoteMSG=data->flag;
    	pthread_mutex_unlock(&(mutexDataControler->mutex));
		if (tmpFlagRemoteMSG == 0) {
			continuThread=0;
		}
		if (is_Motor_Stop(controle_vol->motorsAll3)) {
			continuThread=0;
		}

    	else{
			nanoSleepSecure(nanoSleepTimeIntervalOfSecuritySleep);
		}
    }
    /****************END SECURITY SLEEP*************************/


    if(continuThread){
    	logString("THREAD PID : START");
    }

    while (continuThread) {

    	clock_gettime(CLOCK_MONOTONIC, &t0);//TODO

        if(is_Motor_Stop(controle_vol->motorsAll3)){
        	break;
        }
        
        /*********************************************************/
        /*					CODE FOR GET REMOTE					*/
        iterRemote++;
        if(iterRemote>FREQUENCY_PID/FREQUENCY_CONTROLLER){
        	iterRemote=0;
            pthread_mutex_lock(&(mutexDataControler->mutex));
            if (data->flag== 0) {
                pthread_mutex_unlock(&(mutexDataControler->mutex));
                break;
                //TODO
            }else{
            	mutexDataControler->var = 0;
				powerController[0] = data->axe_Rotation;
				powerController[1] = data->axe_UpDown;
				powerController[2] = data->axe_LeftRight;
				powerController[3] =(-1) * data->axe_FrontBack;
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
		iterBatteryPrint++;
		if (iterBatteryPrint > (FREQUENCY_PID * 2)) {

			float voltageValue=batteryValue * 0.01;

			printf("BATTERY : %f\n",voltageValue );
			iterBatteryPrint = 0;

			pthread_mutex_lock(&(pidInfo->pmutex->mutex));

			pidInfo->battery = voltageValue;

			pthread_mutex_unlock(&(pidInfo->pmutex->mutex));
		}


		if(applyFiltreBatteryValue()){
			logString("THREAD PID : ERROR BATTERY VALUE");
			//TODO
		}

        /************************END BATTERIE****************************/
        
		#ifdef __arm__
        if(imu->IMURead()){
        	readSensorSucces=1;
        }else{
            printf("Capteur fail read.\n");
        	readSensorSucces=0;
        }
		#endif

        if(readSensorSucces){
			#ifdef __arm__
            imuData = imu->getIMUData();
			#endif


            if(testvibration){

                acc_x=imuData.accel.x();
                acc_y=imuData.accel.y();
                acc_z=imuData.accel.z();


    			acc_total_vector[0] = sqrtf((acc_x * acc_x) + (acc_y * acc_y) + (acc_z * acc_z));

    			acc_av_vector = acc_total_vector[0];

    			for (int start = 16; start > 0; start--) {
    				acc_total_vector[start] = acc_total_vector[start - 1];
    				acc_av_vector += acc_total_vector[start];
    			}

    			acc_av_vector /= 17;

    			if (iterVibration < VIBRATION_Moving_average) {
    				iterVibration++;
    				vibration_total_result += fabsf(acc_total_vector[0] - acc_av_vector);
    			} else {
    				iterVibration = 0;
    				lastVibration = vibration_total_result/VIBRATION_Moving_average;
    				vibration_total_result = 0;
    			}
    			
    			iterAccelPrint++;
    			if (iterAccelPrint > (FREQUENCY_PID / 2)) {
    				iterAccelPrint = 0;
    				printf("ACCEL : X : %f  Y : %f  Z : %f | VECTEUR : %f | VIBRATION : %f \n", acc_x, acc_y, acc_z,acc_total_vector[0],lastVibration);
    			}
    			
            }



            /*********************************************************/
            /*					PID                                  */
            
            input_pid_pitch=(input_pid_pitch*0.7) + ((imuData.gyro.y()-gyro_cal[0])*(180/M_PI)*0.3);
            input_pid_roll=(input_pid_pitch*0.7) + ((imuData.gyro.x()-gyro_cal[1])*(180/M_PI)*0.3);
            input_pid_yaw=(input_pid_pitch*0.7) + ((imuData.gyro.z()-gyro_cal[2])*(180/M_PI)*0.3);
            

            //TODO MOTOR_LOW_TIME or MOTOR_MIN_ROTATE_TIME ??
            if(powerController[1]>=0){
                client_gaz=(powerController[1]*4.5) + MOTOR_MIN_ROTATE_TIME;
            }
            else{
                client_gaz = MOTOR_MIN_ROTATE_TIME;
            }
            
            client_pitch=powerController[3] * PID_ANGLE_PRECISION_MULTIPLE;
            client_roll=powerController[2] * PID_ANGLE_PRECISION_MULTIPLE;
            client_yaw=powerController[0] * PID_ANGLE_PRECISION_MULTIPLE;
            
            log_angle_pitch=imuData.fusionPose.y() * RTMATH_RAD_TO_DEGREE;
            client_pitch-= log_angle_pitch * PID_ANGLE_MULTIPLE;
            client_pitch/=3;

            log_angle_roll = imuData.fusionPose.x() * RTMATH_RAD_TO_DEGREE;
            client_roll-= log_angle_roll * PID_ANGLE_MULTIPLE;
            client_roll/=3;
            
            client_yaw/=3;
            
            

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
            //END PID PITCH
            
            //calcule roll PID
            pid_erreur_tmp_roll=input_pid_roll-client_roll;
            pid_accu_erreur_roll+=PID_GAIN_I_ROLL*pid_erreur_tmp_roll;
            if (pid_accu_erreur_roll>PID_MAX_ROLL) {
                pid_accu_erreur_roll=PID_MAX_ROLL;
            }
            else if (pid_accu_erreur_roll < -PID_MAX_ROLL){
                pid_accu_erreur_roll=-PID_MAX_ROLL;
            }
            
            output_pid_roll=PID_GAIN_P_ROLL*pid_erreur_tmp_roll+pid_accu_erreur_roll+PID_GAIN_D_ROLL*(pid_erreur_tmp_roll-pid_last_roll);

            if (output_pid_roll>PID_MAX_ROLL) {
                output_pid_roll=PID_MAX_ROLL;
            }
            else if (output_pid_roll < -PID_MAX_ROLL){
                output_pid_roll=-PID_MAX_ROLL;
            }
            
            pid_last_roll=pid_erreur_tmp_roll;
            //END PID ROLL
            
            //PID YAW
            pid_erreur_tmp_yaw=input_pid_yaw-client_yaw;
            pid_accu_erreur_yaw+=PID_GAIN_I_YAW*pid_erreur_tmp_yaw;
            
            if (pid_accu_erreur_yaw>PID_MAX_YAW) {
                pid_accu_erreur_yaw=PID_MAX_YAW;
            }
            else if (pid_accu_erreur_yaw < -PID_MAX_YAW){
                pid_accu_erreur_yaw=-PID_MAX_YAW;
            }
            
            output_pid_yaw=PID_GAIN_P_YAW*pid_erreur_tmp_yaw+pid_accu_erreur_yaw;
            
            if (output_pid_yaw>PID_MAX_YAW) {
                output_pid_yaw=PID_MAX_YAW;
            }
            else if (output_pid_yaw < -PID_MAX_YAW){
                output_pid_yaw=-PID_MAX_YAW;
            }
            
            
            
            puissance_motor0=client_gaz  - output_pid_pitch + output_pid_roll - output_pid_yaw;
            puissance_motor1=client_gaz + output_pid_pitch + output_pid_roll + output_pid_yaw;
            puissance_motor2=client_gaz + output_pid_pitch - output_pid_roll - output_pid_yaw;
            puissance_motor3=client_gaz - output_pid_pitch - output_pid_roll + output_pid_yaw;
            
		/*
            int puissanceTestPowerGramme=1400;
            puissance_motor0=puissanceTestPowerGramme;
            puissance_motor1=puissanceTestPowerGramme;
            puissance_motor2=puissanceTestPowerGramme;
            puissance_motor3=puissanceTestPowerGramme;
		*/


            /*******************BATTERY MOTOR CALIB**************/
            if(batteryValue<=BATTERY_HIGH_LIMIT && batteryValue>=BATTERY_LOW_LIMIT){
			/*
				int a=10;
            	int b=-20;
				puissance_motor0 +=  ((100 -   ((a*(batteryValue*0.01)) + b))  *puissance_motor0) / 100 ;
				puissance_motor1 +=  ((100 -   ((a*(batteryValue*0.01)) + b))  *puissance_motor1) / 100 ;
				puissance_motor2 +=  ((100 -   ((a*(batteryValue*0.01)) + b))  *puissance_motor2) / 100 ;
				puissance_motor3 +=  ((100 -   ((a*(batteryValue*0.01)) + b))  *puissance_motor3) / 100 ;
				*/
				puissance_motor0 += (1240 - batteryValue) /3500 * puissance_motor0 ;
				puissance_motor1 += (1240 - batteryValue) /3500 * puissance_motor1 ;
				puissance_motor2 += (1240 - batteryValue) /3500 * puissance_motor2 ;
				puissance_motor3 += (1240 - batteryValue) /3500 * puissance_motor3 ;
				
            }
            /*****************END BATTERY MOTOR CALIB*************/


            /*******************SECURITY VALUES MOTORS**************/
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
            
            /****************END SECURITY VALUES MOTORS************/
		/*
            iterPrintPower++;
            if (iterPrintPower > (FREQUENCY_PID * 5)) {
				iterPrintPower=0;
				printf("PUISSANCE : %d %d %d %d \n", puissance_motor0,puissance_motor1,puissance_motor2,puissance_motor3);
			}

		*/
            if(testvibration){
            	powerTab[0] = 1050;
            	powerTab[1] = 1050;
            	powerTab[2] = 1050;
            	powerTab[3] = 1050;
            }else{
            	powerTab[0] = puissance_motor0;
				powerTab[1] = puissance_motor1;
				powerTab[2] = puissance_motor2;
				powerTab[3] = puissance_motor3;
            }

            
            if(modeCalibration){
            	//nothing to apply because we are in a calibrate mode execution
            }else{
            	set_power(controle_vol->motorsAll3,powerTab);
            }
            
            /*************************END PID****************************/
            
            
            /****************************LOG*****************************/
            logTab[0]=powerTab[0];
            logTab[1]=powerTab[1];
            logTab[2]=powerTab[2];
            logTab[3]=powerTab[3];

            logTab[4]=(int)log_angle_pitch;
            logTab[5]=(int)log_angle_roll;

            logTab[6]=(int) (client_pitch * LOG_FLOAT_MULTIPLIER);
            logTab[7]=(int) (client_roll * LOG_FLOAT_MULTIPLIER);
            logTab[8]=(int) (client_yaw  * LOG_FLOAT_MULTIPLIER);

            logTab[9]=(int) (input_pid_pitch  * LOG_FLOAT_MULTIPLIER);
            logTab[10]=(int) (input_pid_roll * LOG_FLOAT_MULTIPLIER);
            logTab[11]=(int) (input_pid_yaw * LOG_FLOAT_MULTIPLIER);

            logTab[12]=(int) (output_pid_pitch * LOG_FLOAT_MULTIPLIER);
            logTab[13]=(int) (output_pid_roll * LOG_FLOAT_MULTIPLIER);
            logTab[14]=(int) (output_pid_yaw * LOG_FLOAT_MULTIPLIER);

            logTab[15]=(int) batteryValue;
            logDataFreq(logTab,nb_values_log);
            /**************************END LOG***************************/
            

        }

        
        /***********SLEEP PID FREQUENCY****************/

        clock_gettime(CLOCK_MONOTONIC, &t1);//TODO
        long timeBetween=t1.tv_nsec - t0.tv_nsec;
        time_t timeSec= t1.tv_sec - t0.tv_sec;
        if(timeSec>0){
        	timeSec *=SEC_TO_NSEC;
        	timeBetween+=timeSec;
        }

		tim.tv_sec = 0;
		tim.tv_nsec = local_period - timeBetween ;
		if(timeBetween<0){

		}
		else if(timeBetween>=local_period){
			sprintf(arrayLog,"THREAD PID : TIME : %ld\n",local_period-timeBetween);
			logString(arrayLog);
		}else{
			//nanoSleepSecure( (local_period-timeBetween) * NSEC_TO_USEC_MULTIPLE);
			clock_nanosleep(CLOCK_MONOTONIC,0,&tim,NULL);
		}
		/***********END SLEEP PID FREQUENCY***********/

    }


endPid:

    set_Motor_Stop(controle_vol->motorsAll3);
    logString("THREAD PID : END");
    return NULL;
}
