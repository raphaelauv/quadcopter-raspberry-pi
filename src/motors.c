#include "motors.h"
#include "concurrent.h"

double frequence=50.0; //frequence du signal d'entré des ESCs
double periode=0; // periode = 1/frequence. Initialisée plus tard.

// Pointeur de fonction qui controle chaque ESC

void * startMoteur(void * args){
    struct motor_info * info=(struct motor_info *)args;
    int low,hight;
    if (wiringPiSetup () == -1){
        return NULL;
    }
    pinMode (info->broche, OUTPUT); //On defini la sorti du signal
    while(1){
        //On Bloc le Mutex, on copie les valeurs info->high_time et info->low_time pour pas resté avec le mutex bloquée.
        pthread_mutex_lock(&info->lock);
        if(!info->bool_arret_moteur){//Dans le cas on est pas dans une demande d'arret moteur.
        	hight=(int)info->high_time;
        	low=(int)info->low_time;
        	pthread_mutex_unlock(&info->lock);
        	digitalWrite(info->broche, 1);       // Etat haut du signal
        	usleep((int)hight);
        	digitalWrite(info->broche,0);         //Etat bas du signal.
        	usleep((int)(low));
        }
        else{//ARRET des moteurs d'urgence demandé.
        	break;
        }
    }
}

void init_motor_info(struct motor_info *info,int broche){
    if(periode<=0){//Si la periode n'est pas Initialisé
        printf("Fatal erreur:Periode don't initialize");
        exit(1);
    }
    info->bool_arret_moteur=0;
    info->broche=broche;
    info->high_time=(periode*5.0/100.0);;//Correspond a 0% de puissance .(1/Frequence * 5/100)= 1 dans notre cas.
    info->low_time=periode-info->high_time;// le reste de la periode.
    info->lock=(pthread_mutex_t )PTHREAD_MUTEX_INITIALIZER;;
}

int set_power(struct  motor_info * info,double power){
    int a=(int)power;
    if(a<5 || a>10){
        return 1;
    }
    pthread_mutex_lock(&info->lock);
    info->high_time=(periode*power/100.0); // On calcule le nouveaux rapport cyclique.
    info->low_time=periode-info->high_time; //
    pthread_mutex_unlock(&info->lock);
    printf("%i\n",a);
    return 0;
}

void init_motors(struct motor_info * info_m0,struct motor_info * info_m1,struct motor_info * info_m2,struct motor_info * info_m3){
    cpu_set_t cpuset;//ensemble des CPU utilisable.
    pthread_t thr;
    pthread_attr_t attributs;
    struct sched_param parametres;
    int m0,m1,m2,m3;

    //Definir la taille de la memoire virtuelle pour que le kernel de fasse pas d'allocation dynamique.
    mlockall(MCL_CURRENT | MCL_FUTURE);
    
    //init broche du signal du controle des moteur
    m0=5;
    m1=28;
    m2=2;
    m3=24;
    
    //init 0% de puissance des moteur en fonction de la frequence
    periode=(1.0/frequence)*1000000;
    init_motor_info(info_m0,m0);
    init_motor_info(info_m1,m1);
    init_motor_info(info_m2,m2);
    init_motor_info(info_m3,m3);
    
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
    pthread_create(&thr,& attributs,startMoteur,(void *)info_m0);
    pthread_create(&thr,& attributs,startMoteur,(void *)info_m1);
    pthread_create(&thr,& attributs,startMoteur,(void *)info_m2);
    pthread_create(&thr,& attributs,startMoteur,(void *)info_m3);
    
    pthread_attr_destroy(&attributs);//Libere les resource.

}

