#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <errno.h>
#include <unistd.h>

#define L3GD20H_I2C_ADDRESS 0x6B // adresse I2C du gyroscope
#define L3GD20H_OUT_X_L 0x28
#define L3GD20H_OUT_X_H 0x29
#define L3GD20H_OUT_Y_L 0x2A
#define L3GD20H_OUT_Y_H 0x2B
#define L3GD20H_OUT_Z_L 0x2C
#define L3GD20H_OUT_Z_H 0x2D

int main(){

  int fd = wiringPiI2CSetup(L3GD20H_I2C_ADDRESS);
  if(fd == -1){
    perror("Setup\n");
    return 1;
  }
  printf("ok\n");

  float x,y,z;
  
  while(1){
    x = wiringPiI2CReadReg16(fd, L3GD20H_OUT_X_H);
    y = wiringPiI2CReadReg16(fd, L3GD20H_OUT_Y_H);
    z = wiringPiI2CReadReg16(fd, L3GD20H_OUT_Z_H);

    printf("x=%f\ny=%f\nz=%f\n",x,y,z);
    sleep(1);
  }
  
  
  return 0;
}
