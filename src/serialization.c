#include <stdlib.h>


MessageToStruc(char * message,int sizeFloat){

	float a=strtof(message,0);

	float b=strtof(message+sizeFloat-1,0);

	printf ("float a = %.6f\n", a);

	printf ("float b = %.6f\n", b);
}
char * structToMessage(int sizeFloat){

	float num = 9.123456789;
	float num2 = 5.123456789;

	char * output=malloc(sizeof(char)*(sizeFloat*4));

	snprintf(output, sizeFloat, "%f", num);

	output[sizeFloat-2]=' ';

	snprintf(output+sizeFloat-1, sizeFloat, "%f", num2);

	printf("%s\n", output);

	return output;

}

int main(){
	char * result=structToMessage(10);

	MessageToStruc(result,10);

}
