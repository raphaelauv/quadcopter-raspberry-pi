#include "PID.hpp"


int init_args_PID(args_PID ** argPID,DataController * dataControl,MotorsAll2 * motorsAll2){
    
    *argPID =(args_PID *) malloc(sizeof(args_PID));
    if (*argPID == NULL) {
        logString("MALLOC FAIL : init_args_PID");
        return EXIT_FAILURE;
    }
    
    (*argPID)->dataController = dataControl;
    (*argPID)->motorsAll2 = motorsAll2;
    
#ifdef __arm__
    RTIMU *imu;
    imu = capteurInit();
    
    if(imu==NULL){
        logString("THREAD MAIN : ERROR NEW FAIL RTIMU ->imu");
        return EXIT_FAILURE;
    }else{
        logString("THREAD MAIN : CAPTEUR INIT SUCCES");
        (*argPID)->imu=imu;
    }
#endif
    
    return 0;
}

void clean_args_PID(args_PID * arg) {
    if (arg != NULL) {
        clean_MotorsAll2(arg->motorsAll2);
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
int init_thread_PID(pthread_t * threadPID,args_PID * argPID){
    
    pthread_attr_t attributs;
    if(init_Attr_Pthread(&attributs,99,CPU_CORE_PID)){
        logString("THREAD MAIN : ERROR pthread_attributs PID");
        return -1;
    }
    
    int result=pthread_create(threadPID, &attributs, thread_PID, argPID);
    if (result) {
        logString("THREAD MAIN : ERROR pthread_create PID");
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
    if(val<1100){
        val=1100;
    }
    else if(val>2000){
        val=2000;
    }
    return val;
}

void * thread_PID(void * args){
    
    //test();
    //calibrate(args);
    args_PID  * controle_vol =(args_PID  *)args;
    DataController * data = controle_vol->dataController;
    PMutex * mutexDataControler =controle_vol->dataController->pmutex;
    RTIMU *imu =(RTIMU *)controle_vol->imu;
    //char array[400];
    //sprintf(array, "VAL POINT BOOL ARRET IN PID   : %d\n",controle_vol->motorsAll2->bool_arret_moteur);
    //logString(array);
    int powerTab[NUMBER_OF_MOTORS];
    int powerController[NUMBER_OF_MOTORS];
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
    
    
    RTIMU_DATA imuData;
    
    if(setDataFrequence(50)){
        printf("ERROR\n");
        //TODO
    }
    
    int continuThread=1;
    
    float gyro_cal[3]={0,0,0};
    for (i=0; i<2000; i++) {
        if(imu->IMURead()){
            m_imuData = imu->getIMUData();
            gyro_cal[0]+=m_imuData.gyro.x();
            gyro_cal[1]+=m_imuData.gyro.y();
            gyro_cal[2]+=m_imuData.gyro.z();
        }
        else{
            printf("trop rapide");
        }
        usleep(2000);
    }
    gyro_cal[0]/=2000;
    gyro_cal[1]/=2000;
    gyro_cal[2]/=2000;
    printf("Calibration fini\n");
    
    /*********************************************************/
    /*				START PID SECURITY SLEEP				*/
    int numberOfSecondSleep=0;
    char tmpFlag=0;
    logString("THREAD PID : SECURITY SLEEP");
    while(numberOfSecondSleep<PID_SLEEP_TIME_SECURITE*10){
    	numberOfSecondSleep++;
    	pthread_mutex_lock(&(mutexDataControler->mutex));
    	tmpFlag=data->flag;
    	pthread_mutex_unlock(&(mutexDataControler->mutex));
		if (tmpFlag == 0) {
			setMotorStop(controle_vol->motorsAll2);
			continuThread = 0;
			break;
		}
    	else{
			usleep(1000000/10);
		}
    }
    /*********************************************************/

    struct timeval tv;
    int cmp=0;
    int timeUsecStart=0;
    int timeUsecEnd=0;
    int timeBetween=0;

    if(continuThread){
    	logString("THREAD PID : START");
    }
    while (continuThread) {

        gettimeofday(&tv, NULL);
        timeUsecStart= (int)tv.tv_sec * USEC_TO_SEC + (int)tv.tv_usec;

        if(isMotorStop(controle_vol->motorsAll2)){
        	continuThread = 0;
        	continue;
        }
        
        /*********************************************************/
        /*					CODE FOR GET REMOTE					*/
        
        if(cmp==50){
        	cmp=0;
            pthread_mutex_lock(&(mutexDataControler->mutex));
            if (data->flag== 0) {
                pthread_mutex_unlock(&(mutexDataControler->mutex));
                //printf("VAL POINT BOOL ARRET IN PID   : %d\n",controle_vol->motorsAll2->bool_arret_moteur);
                setMotorStop(controle_vol->motorsAll2);
                continuThread = 0;
                continue;
            }
            
            mutexDataControler->var = 0;
            powerController[0] = data->axe_Rotation;
            powerController[1] = data->axe_UpDown;
            powerController[2] = data->axe_LeftRight;
            powerController[3] = data->axe_FrontBack;
            pthread_mutex_unlock(&(mutexDataControler->mutex));
            /*
            powerTab[0] =absValue(powerTab[0]);
            powerTab[1] =absValue(powerTab[1]);
            powerTab[2] =absValue(powerTab[2]);
            powerTab[3] =absValue(powerTab[3]);

            set_power2(controle_vol->motorsAll2,powerTab);
            */
        }
        else{
            cmp++;
        }
        /*********************************************************/
        
#ifdef __arm__
        if(imu->IMURead()){
            imuData = imu->getIMUData();
            //input PID
            input_pid_pitch=(input_pid_pitch*0.7) + ((imuData.gyro.x()-gyro_cal[0])*(180/M_PI)*0.3);
            
            if(powerController[1]>=0){
                client_gaz=powerController[1]*7+1100;
            }
            else{
                client_gaz=1100;
            }
            
            client_pitch=powerController[2]*5;
            client_pitch-=imuData.fusionPose.y()*RTMATH_RAD_TO_DEGREE)*15;
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
            if(puissance_motor0<1100) puissance_motor0=1100;
            if(puissance_motor1<1100) puissance_motor1=1100;
            
            //puissance max=MOTOR_HIGH_TIME donc il faut pas depasser.
            if(puissance_motor0>MOTOR_HIGH_TIME) puissance_motor0=MOTOR_HIGH_TIME;
            if(puissance_motor1>MOTOR_HIGH_TIME) puissance_motor1=MOTOR_HIGH_TIME;
            
            powerTab[0] = puissance_motor0;
            powerTab[1] = puissance_motor1;
            powerTab[3] = 1000;
            powerTab[4] = 1000;
            //powerTab[2] = puissance_motor2;
            //powerTab[3] = puissance_motor3;
            set_power2(controle_vol->motorsAll2,powerTab);
            logDataFreq(powerTab,NUMBER_OF_MOTORS);
            
        }
        
#endif
        
        /*********************************************************/
        /*			CODE FOR SLEEP PID FREQUENCY				*/
        
        gettimeofday(&tv, NULL);
        timeUsecEnd= (int)tv.tv_sec * USEC_TO_SEC + (int)tv.tv_usec;
        timeBetween=timeUsecEnd - timeUsecStart;
        
        if(timeBetween<0){
        }
        else if(timeBetween>=local_period){
            printf("PID temps sleep : %d\n",local_period-timeBetween);
        }else{
            usleep(local_period-timeBetween);
        }
    }
    logString("THREAD PID : END");
    return NULL;
}
