#include "motors.h"
void viderBuffer(void){
    int c;
    while((c=getchar()) != EOF && c != '\n');
}

int main(){
    pthread_mutex_t lock_m0 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t lock_m1 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t lock_m2 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t lock_m3 = PTHREAD_MUTEX_INITIALIZER;
    struct motor_info info_m0,info_m1,info_m2,info_m3;
    init_motors(& info_m0,& info_m1,& info_m2,& info_m3, & lock_m0,& lock_m1,& lock_m2,& lock_m3);
    char a;
    double vitesse;
    while (1) {
        printf("Salut change quelque chose nan?");
        scanf("%c", &a);
        
        scanf("%lf",&vitesse);
        if(a=='1'){
            if(set_power(& info_m0,vitesse))
                printf("Erreur\n");
        }
        else if(a=='2'){
            if(set_power(& info_m1,vitesse))
                printf("Erreur");
        }
        else if(a=='3'){
            if(set_power(& info_m2,vitesse))
                printf("Erreur");
        }
        else if(a=='4'){
            if(set_power(& info_m3,vitesse))
                printf("Erreur");
        }
        viderBuffer();
    }
    return 0;
}