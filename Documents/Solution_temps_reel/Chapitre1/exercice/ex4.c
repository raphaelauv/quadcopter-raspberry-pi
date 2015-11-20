#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
  int pid=fork();
  if(pid==-1){
    perror("fork");
    exit(1);
  }
  if(pid==0){
    sleep(10);
    exit(0);
  }
  else{
    while(1){
      sleep(1);
    }
  }
	       
}
