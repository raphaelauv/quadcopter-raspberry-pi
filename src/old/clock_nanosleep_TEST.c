#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <complex.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>


#define SEC_TO_NSEC 1000 * 1000 * 1000

int main(){

  long local_period=(1.0/2.0) *SEC_TO_NSEC;
  while(1){
    printf("----------------------\n");
      

      //printf("%d\n",(1000 * 1000 * 1000 )-1);
      //printf("%ld\n",local_period);

      
      struct timespec t0, t1;
      struct timespec tim, tim2;

      tim.tv_sec = 0;
      tim.tv_nsec = 1000;

      clock_gettime(CLOCK_MONOTONIC, &t0);

      nanosleep(&tim,NULL);

      clock_gettime(CLOCK_MONOTONIC, &t1);

      /*
      printf("Repaint done in %.9 lfs\n",
               ((double)t1.tv_sec - t0.tv_sec) +
               ((double)t1.tv_nsec - t0.tv_nsec) / 1.0E9);

      */

      double a= ((double)t1.tv_sec - t0.tv_sec);
      long diff;
      if(a==0){
        long diff=t1.tv_nsec - t0.tv_nsec;
        printf("ACTION TOOK in %ld \n",diff);


      tim2.tv_sec = 0;
      tim2.tv_nsec = local_period - diff ;

      printf("LET SLEEP %ld \n",tim2.tv_nsec);
      clock_nanosleep(CLOCK_MONOTONIC,0,&tim2,NULL);


      }else{
        printf("ERROR\n");
      }


  }
}
