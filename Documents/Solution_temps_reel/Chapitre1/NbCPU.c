#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	printf("nombre de CPU %lu\n", sysconf(_SC_NPROCESSORS_ONLN));
	return 0;
}
