#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
  int i=0;
  while(1){
    i++;
    if(i==1000000){
      sleep(5);
      i=0;
    }
  }
  return 0;
}
