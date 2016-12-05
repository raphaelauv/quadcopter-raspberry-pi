#include "motors.h"
double frequence=50.0;
double periode=0;

void * moteur(void * args){
    struct motor_info * info=(struct motor_info *)args;
    int low,hight;
    if (wiringPiSetup () == -1)
        return;
    pinMode (info->broche, OUTPUT);
    for (;;){
        //On Bloc le Mutex, on copie les valeurs info->high_time et info->low_time pour pas resté avec le mutex bloquée.
        pthread_mutex_lock(info->lock);
        hight=(int)info->high_time;
        low=(int)info->low_time;
        pthread_mutex_unlock(info->lock);
        digitalWrite(info->broche, 1);       // On
        usleep((int)hight);
        digitalWrite(info->broche,0);         //Off
        usleep((int)(low));
    }
}

void init_motor_info(struct motor_info *info,int broche, pthread_mutex_t * lock1){
    if(periode<=0){
        printf("Fatal erreur:Periode don't initialize");
        exit(1);
    }
    info->broche=broche;
    info->high_time=(periode*5.0/100.0);;
    info->low_time=periode-info->high_time;
    info->lock=lock1;
}

int set_power(struct  motor_info * info,double power){
    int a=(int)power;
    if(a<5 || a>10){
        return 1;
    }
    pthread_mutex_lock(info->lock);
    info->high_time=(periode*power/100.0);
    info->low_time=periode-info->high_time;
    pthread_mutex_unlock(info->lock);
    printf("%i\n",a);
    return 0;
}

void init_motors(struct motor_info * info_m0,struct motor_info * info_m1,struct motor_info * info_m2,struct motor_info * info_m3,
    pthread_mutex_t * lock_m0,pthread_mutex_t * lock_m1,pthread_mutex_t * lock_m2,pthread_mutex_t * lock_m3){
    cpu_set_t cpuset;//ensemble des CPU utilisable.
    pthread_t thr;
    pthread_attr_t attributs;
    struct sched_param parametres;
    int m0,m1,m2,m3;
    double vitesse;
    //Definir la taille de la memoire virtuelle pour que le kernel de fasse pas d'allocation dynamique.
    mlockall(MCL_CURRENT | MCL_FUTURE);
    
    //init broche du signal du controle des moteur
    m0=5;
    m1=28;
    m2=2;
    m3=24;
    
    //init 0% de puissance des moteur en fonction de la frequence
    periode=(1.0/frequence)*1000000;
    init_motor_info(info_m0,m0,lock_m0);
    init_motor_info(info_m1,m1,lock_m1);
    init_motor_info(info_m2,m2,lock_m2);
    init_motor_info(info_m3,m3,lock_m3);
    
    //init avec les attributs par defaut.
    pthread_attr_init(& attributs);
    
    //Mettre Sur un coeur ici le coeur 0;
    CPU_ZERO(& cpuset);//mes lensemble a vide
    CPU_SET(0,& cpuset);//ajoute i a lensemble des CPU
    //fixer l'affinity
    pthread_attr_setaffinity_np(& attributs, sizeof(cpu_set_t), & cpuset);
    
    //Priorite temps reel.
    parametres.sched_priority=99; //choisir le prioroté (de 0 a 99)
    pthread_attr_setschedpolicy(&attributs,SCHED_FIFO);//Inscrire le type d ordonnancement voulu dans les attribue.
    pthread_attr_setschedparam(&attributs,&parametres); //incrire la priorite dans les attributs.
    pthread_attr_setinheritsched(&attributs,PTHREAD_EXPLICIT_SCHED); //chaque aura sa propre priorité.
    pthread_attr_setscope(&attributs,PTHREAD_SCOPE_SYSTEM); // Pour ne pas etre preemte par des processus du system
    
    //creation du thread.
    pthread_create(&thr,& attributs,moteur,(void *)info_m0);
    pthread_create(&thr,& attributs,moteur,(void *)info_m1);
    pthread_create(&thr,& attributs,moteur,(void *)info_m2);
    pthread_create(&thr,& attributs,moteur,(void *)info_m3);
    
    pthread_attr_destroy(&attributs);//Libere les resource.

}