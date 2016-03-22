/*
            +-----+-----+---------+------+---+---Pi 2---+---+------+---------+-----+-----+
            | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
            +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
            |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
            |   2 |   8 |   SDA.1 | ALT0 | 1 |  3 || 4  |   |      | 5V      |     |     |
            |   3 |   9 |   SCL.1 | ALT0 | 1 |  5 || 6  |   |      | 0v      |     |     |
            |   4 |   7 | GPIO. 7 |   IN | 1 |  7 || 8  | 1 | ALT0 | TxD     | 15  | 14  |
        ----|     |     |      0v |      |   |  9 || 10 | 1 | ALT0 | RxD     | 16  | 15  |
        |   |  17 |   0 | GPIO. 0 |   IN | 0 | 11 || 12 | 0 | IN   | GPIO. 1 | 1   | 18  |
  Motor 3<--|  27 |   2 | GPIO. 2 |   IN | 0 | 13 || 14 |   |      | 0v      |     |     |----
            |  22 |   3 | GPIO. 3 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO. 4 | 4   | 23  |   |
            |     |     |    3.3v |      |   | 17 || 18 | 0 | IN   | GPIO. 5 | 5   | 24  |--->Motor 1
            |  10 |  12 |    MOSI | ALT0 | 0 | 19 || 20 |   |      | 0v      |     |     |
            |   9 |  13 |    MISO | ALT0 | 0 | 21 || 22 | 0 | IN   | GPIO. 6 | 6   | 25  |
            |  11 |  14 |    SCLK | ALT0 | 0 | 23 || 24 | 1 | ALT0 | CE0     | 10  | 8   |
            |     |     |      0v |      |   | 25 || 26 | 1 | ALT0 | CE1     | 11  | 7   |
            |   0 |  30 |   SDA.0 |   IN | 1 | 27 || 28 | 1 | IN   | SCL.0   | 31  | 1   |
            |   5 |  21 | GPIO.21 |   IN | 1 | 29 || 30 |   |      | 0v      |     |     |
            |   6 |  22 | GPIO.22 |   IN | 1 | 31 || 32 | 0 | IN   | GPIO.26 | 26  | 12  |
            |  13 |  23 | GPIO.23 |   IN | 0 | 33 || 34 |   |      | 0v      |     |     |----
 Motor 4<---|  19 |  24 | GPIO.24 |   IN | 0 | 35 || 36 | 0 | IN   | GPIO.27 | 27  | 16  |   |
        |   |  26 |  25 | GPIO.25 |   IN | 0 | 37 || 38 | 0 | IN   | GPIO.28 | 28  | 20  |--->Motor 2
        |---|     |     |      0v |      |   | 39 || 40 | 0 | IN   | GPIO.29 | 29  | 21  |
            +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
            | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
            +-----+-----+---------+------+---+---Pi 2---+---+------+---------+-----+-----+
 */



#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/mman.h>



//variable global
double frequence=50.0;
double periode=0;
struct motor_info{
    int broche;
    double high_time;
    double low_time;
};

struct motor_info info_m1,info_m2,info_m3,info_m4;
static pthread_mutex_t lock_m1 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock_m2 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock_m3 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock_m4 = PTHREAD_MUTEX_INITIALIZER;



void * moteur_1 (void * args){
    struct motor_info * info=(struct motor_info *)args;
    if (wiringPiSetup () == -1)
        return;
    pinMode (info->broche, OUTPUT);
    int high, low;
    for (;;){
      high = (int)info->high_time;
      low = (int)info->low_time;
      digitalWrite(info->broche, 1) ;       // On
      usleep(high);
      digitalWrite(info->broche,0);         //off
      usleep(low);
    }
}
void * moteur_2 (void * args){
    struct motor_info * info=(struct motor_info *)args;
    if (wiringPiSetup () == -1)
        return;
    pinMode (info->broche, OUTPUT);
int high, low;
    for (;;){
      high = (int)info->high_time;
      low = (int)info->low_time;
      digitalWrite(info->broche, 1) ;       // On
      usleep(high);
      digitalWrite(info->broche,0);         //off
      usleep(low);
    }
}
void * moteur_3 (void * args){
    struct motor_info * info=(struct motor_info *)args;
    if (wiringPiSetup () == -1)
        return;
    pinMode (info->broche, OUTPUT);
    int high, low;
    for (;;){
      high = (int)info->high_time;
      low = (int)info->low_time;
      digitalWrite(info->broche, 1) ;       // On
      usleep(high);
      digitalWrite(info->broche,0);         //off
      usleep(low);
    }
}
void * moteur_4 (void * args){
    struct motor_info * info=(struct motor_info *)args;
    if (wiringPiSetup () == -1)
        return;
    pinMode (info->broche, OUTPUT);
    int high, low;
    for (;;){
      high = (int)info->high_time;
      low = (int)info->low_time;
      digitalWrite(info->broche, 1) ;       // On
      usleep(high);
      digitalWrite(info->broche,0);         //off
      usleep(low);
    }
}
//init les parametre des moteur, le numero de broche le temps d'etat haut a 5%(donc 0 pour cent de puissance) et le temps bas (en microS), il faut definir la periode au paravant.
void init_motor_info(struct  motor_info *info,int broche){
    if(periode<=0){
        printf("Fatal erreur:Periode don't initialize");
        exit(1);
    }
    info->broche=broche;
    info->high_time=(periode*5.0/100.0);;
    info->low_time=periode-info->high_time;
}
//change la puissance du moteur, atention aucune verification de power et faite.
void set_power(struct  motor_info * info,double power){
    info->high_time=(periode*power/100.0);
    info->low_time=periode-info->high_time;
}
void viderBuffer(void){
    int c;
    while((c=getchar()) != EOF && c != '\n');
}
int main(){
    char a;
    cpu_set_t cpuset;//ensemble des CPU utilisable.
    pthread_t thr;
    pthread_attr_t attributs;
    struct sched_param parametres;
    int m1,m2,m3,m4;
    double vitesse;
    struct motor_info info_m1,info_m2,info_m3,info_m4;
    //Definir la taille de la memoire virtuelle pour que le kernel de fasse pas d'allocation dynamique.
    mlockall(MCL_CURRENT | MCL_FUTURE);
    
    //init broche du signal du controle des moteur
    m1=5;
    m2=28;
    m3=3;
    m4=24;
    
    //init 0% de puissance des moteur en fonction de la frequence
    periode=(1.0/frequence)*1000000;
    init_motor_info(& info_m1,m1);
    init_motor_info(& info_m2,m2);
    init_motor_info(& info_m3,m3);
    init_motor_info(& info_m4,m4);
    
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
    pthread_create(&thr,& attributs,moteur_1,(void *)&info_m1);
    pthread_create(&thr,& attributs,moteur_2,(void *)&info_m2);
    pthread_create(&thr,& attributs,moteur_3,(void *)&info_m3);
    pthread_create(&thr,& attributs,moteur_4,(void *)&info_m4);
    
    pthread_attr_destroy(&attributs);//Libere les resource.
    while (1) {
        scanf("%c", &a);
        
        scanf("%lf",&vitesse);
        if(a=='1'){
            set_power(& info_m1,vitesse);
        }
        else if(a=='2'){
            set_power(& info_m2,vitesse);
        }
        else if(a=='3'){
            set_power(& info_m3,vitesse);
        }
        else if(a=='4'){
            set_power(& info_m4,vitesse);
        }
        printf("Moteur1:\nBroche:%d\nHigh:%lf\nLow:%lf\n\n",info_m1.broche,info_m1.high_time,info_m1.low_time);
        printf("Moteur2:\nBroche:%d\nHigh:%lf\nLow:%lf\n\n",info_m2.broche,info_m2.high_time,info_m2.low_time);
        printf("Moteur3:\nBroche:%d\nHigh:%lf\nLow:%lf\n\n",info_m3.broche,info_m3.high_time,info_m3.low_time);
        printf("Moteur4:\nBroche:%d\nHigh:%lf\nLow:%lf\n\n",info_m4.broche,info_m4.high_time,info_m4.low_time);
        viderBuffer();
    }
    return 0;
}
