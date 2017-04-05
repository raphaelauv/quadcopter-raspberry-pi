#include <stdio.h>
#include <wiringPiSPI.h>
#include <unistd.h>


/*
 void setup() {
 // put your setup code here, to run once:
 Serial.begin(9600);
 battery_voltage = (analogRead(0) + 65) * 1.2317;
 }
 
 void loop() {
 // put your main code here, to run repeatedly:
 battery_voltage = battery_voltage * 0.92 + (analogRead(0) + 65) * 0.09853;
 float voltage=(5.0*analogRead(0))/1023.0;
 float voltage_batterie=(battery_voltage * 0.01)-0.16;
 Serial.print(voltage_batterie);
 Serial.print("\n");
 delay(time_to_sleep);
 }
 */
int main(int argc, char **argv){
    if(wiringPiSPISetup (0, 300000)==-1){
        printf("Setup faut\n");
        return 1;
    }
    int i;
    int j=0;
    unsigned char send[3]={0x18,0x00,0x00};
    int voltage=0;
    float batterie_charge=0;
    while (1) {
        if(!wiringPiSPIDataRW (0, send, 3)){
            printf("probleme\n");
        }
        voltage=((send[1] & 0x03) <<8) | send[2];
        batterie_charge=batterie_charge* 0.92 + (voltage+65)* 0.09853;
        if(j==125){
            printf("Charge batterie = %f\n",batterie_charge*0.01 - 0.5);
            j=0;
        }
        send[0]=0x01;
        send[1]=0x80;
        usleep(4000);
        j++;
    }
    printf("FINI\n");
    return 0;
}