#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
  char * message="Message\n";
  write(STDOUT_FILENO,message, strlen(message));
  return 0;
}
