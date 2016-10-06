#include <stdio.h>
#include <stdlib.h>

int main(){
  char * pointeur= NULL;
  fprintf(stderr,"avant ecriture dans le pointeur\n");
  pointeur[0]='X';
  fprintf(stderr,"apres erreur");
  return 0;
}
