/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Name        		: PCA9685.c
 * Author      		: raphaelauv
 * Original Author  : Georgi Todorov <terahz@geodar.com>
 * Created on  		: Apr 3, 2017
 *
 * Copyright © 2017 Raphaelauv
 */


#include "PCA9685.h"

int mode_flag=0;

void setMode_PCA9685(int flag){
	mode_flag=flag;
}

//! Constructor takes bus and address arguments
/*!
 \param bus the bus to use in /dev/i2c-%d.
 \param address the device address on bus
 */
int initPCA9685(PCA9685 ** pca,int bus, int address){

	*pca = (PCA9685 *) malloc(sizeof(PCA9685));
	if (*pca == NULL) {
		logString("MALLOC FAIL : PCA9685");
		return -1;
	}

	if(mode_flag==FLAG_WIRINGII2C){

		#ifdef __arm__
		(*pca)->WiringPiI2C_fd=wiringPiI2CSetup(bus);
		if((*pca)->WiringPiI2C_fd<0){
			logString("wiringPiI2CSetup FAIL : PCA9685");
			return -1;
		}
		#endif

	}else if(mode_flag==FLAG_CUSTOM_I2C){

		#ifdef __arm__
		I2C_custom * i2c_c;
		if(initI2C_custom(&i2c_c,bus,address)){
			return -1;
		}
		(*pca)->i2c =i2c_c;
		if(PCA9685_reset((*pca))){
			return -1;
		}
		if(PCA9685_setPWMFreq((*pca),PCA9685_FREQUENCY)){
			return -1;
		}
		#endif

	}else{
		return -1;
	}

	return 0;
}

void cleanPCA9685(PCA9685 *pca){
	if(pca!=NULL){
		cleanI2C_custom(pca->i2c);
	}
	free(pca);
	pca = NULL;
}

//! Sets PCA9685 mode to 00
int PCA9685_reset(PCA9685 *pca) {
		int result=0;

		if(mode_flag==FLAG_WIRINGII2C){
			#ifdef __arm__
			result+=wiringPiI2CWriteReg8(pca->WiringPiI2C_fd,MODE1,0x00);
			result+=wiringPiI2CWriteReg8(pca->WiringPiI2C_fd,MODE2, 0x04);
			#endif
		}
		else if(mode_flag==FLAG_CUSTOM_I2C){
			result+=I2C_custom_write_byte(pca->i2c,MODE1, 0x00); //Normal mode
			result+=I2C_custom_write_byte(pca->i2c,MODE2, 0x04); //totem pole
		}

		if(result){
			logString("PCA9685 FAIL : PCA9685_reset");
			return -1;
		}
		return 0;
}
//! Set the frequency of PWM
/*!
 \param freq desired frequency. 40Hz to 1000Hz using internal 25MHz oscillator.
 */
int PCA9685_setPWMFreq(PCA9685 *pca,int freq) {

		uint8_t prescale_val = (CLOCK_FREQ / 4096 / freq)  - 1;
		int result=0;

		if(mode_flag==FLAG_WIRINGII2C){
			#ifdef __arm__
			result+=wiringPiI2CWriteReg8(pca->WiringPiI2C_fd,MODE1,0x10);
			result+=wiringPiI2CWriteReg8(pca->WiringPiI2C_fd,PRE_SCALE, prescale_val);
			result+=wiringPiI2CWriteReg8(pca->WiringPiI2C_fd,MODE1, 0x80);
			result+=wiringPiI2CWriteReg8(pca->WiringPiI2C_fd,MODE2, 0x04);
			#endif

		}
		else if(mode_flag==FLAG_CUSTOM_I2C){
			result+=I2C_custom_write_byte(pca->i2c,MODE1, 0x10); //sleep
			result+=I2C_custom_write_byte(pca->i2c,PRE_SCALE, prescale_val); // multiplyer for PWM frequency
			result+=I2C_custom_write_byte(pca->i2c,MODE1, 0x80); //restart
			result+=I2C_custom_write_byte(pca->i2c,MODE2, 0x04); //totem pole (default)
		}

		if(result){
			logString("PCA9685 FAIL : PCA9685_setPWMFreq");
			return -1;
		}
		return 0;
}

//! PWM a single channel
/*!
 \param led channel (1-16) to set PWM value for
 \param value 0-4095 value for PWM
 */
int PCA9685_setPWM_1(PCA9685 *pca,uint8_t led, int value) {
	return PCA9685_setPWM_2(pca,led, 0, value);
}
//! PWM a single channel with custom on time
/*!
 \param led channel (1-16) to set PWM value for
 \param on_value 0-4095 value to turn on the pulse
 \param off_value 0-4095 value to turn off the pulse
 */
int PCA9685_setPWM_2(PCA9685 *pca,uint8_t led, int on_value, int off_value) {

		int result=0;

		/* NOT WORKING
		uint8_t val0=on_value & 0xFF;
		uint8_t val1=on_value >> 8;
		uint8_t val2=off_value & 0xFF;
		uint8_t val3=off_value >> 8;
		uint8_t size=4;
		uint8_t address_Array[size]={LED0_ON_L + LED_MULTIPLYER,LED0_ON_H + LED_MULTIPLYER,LED0_OFF_L + LED_MULTIPLYER,LED0_OFF_H + LED_MULTIPLYER};
		uint8_t data_Array[size]={val0,val1,val2,val3};
		result=I2C_custom_write_multiple_byte(pca->i2c,address_Array,data_Array,size);
		*/

		if(mode_flag==FLAG_WIRINGII2C){
			#ifdef __arm__
			result+=wiringPiI2CWriteReg8(pca->WiringPiI2C_fd,LED0_ON_L + LED_MULTIPLYER * (led - 1), on_value & 0xFF);
			result+=wiringPiI2CWriteReg8(pca->WiringPiI2C_fd,LED0_ON_H + LED_MULTIPLYER * (led - 1), on_value >> 8);
			result+=wiringPiI2CWriteReg8(pca->WiringPiI2C_fd,LED0_OFF_L + LED_MULTIPLYER * (led - 1), off_value & 0xFF);
			result+=wiringPiI2CWriteReg8(pca->WiringPiI2C_fd,LED0_OFF_H + LED_MULTIPLYER * (led - 1), off_value >> 8);
			#endif
		}
		else if(mode_flag==FLAG_CUSTOM_I2C){
			result+=I2C_custom_write_byte(pca->i2c,LED0_ON_L + LED_MULTIPLYER * (led - 1), on_value & 0xFF);
			result+=I2C_custom_write_byte(pca->i2c,LED0_ON_H + LED_MULTIPLYER * (led - 1), on_value >> 8);
			result+=I2C_custom_write_byte(pca->i2c,LED0_OFF_L + LED_MULTIPLYER * (led - 1), off_value & 0xFF);
			result+=I2C_custom_write_byte(pca->i2c,LED0_OFF_H + LED_MULTIPLYER * (led - 1), off_value >> 8);
		}

		if(result){
			logString("PCA9685 FAIL : PCA9685_setPWM_2");
			return -1;
		}
		return 0;
}

//! Get current PWM value
/*!
 \param led channel (1-16) to get PWM value from
 */
int PCA9685_getPWM(PCA9685 *pca,uint8_t led){
	int ledval = 0;
	if(mode_flag==FLAG_WIRINGII2C){
		#ifdef __arm__
		ledval = wiringPiI2CReadReg8(pca->WiringPiI2C_fd, LED0_OFF_H + LED_MULTIPLYER * (led-1));
		#endif
	}
	else if(mode_flag==FLAG_CUSTOM_I2C){
		ledval = I2C_custom_read_byte(pca->i2c,LED0_OFF_H + LED_MULTIPLYER * (led-1));
	}

	if(ledval==-1){
		logString("PCA9685 FAIL : PCA9685_getPWM");
		return -1;
	}
	ledval = ledval & 0xf;
	ledval <<= 8;

	int tmp=0;
	if(mode_flag==FLAG_WIRINGII2C){
		#ifdef __arm__
		tmp = wiringPiI2CReadReg8(pca->WiringPiI2C_fd,LED0_OFF_L + LED_MULTIPLYER * (led-1));
		#endif
	}
	else if(mode_flag==FLAG_CUSTOM_I2C){
		tmp = I2C_custom_read_byte(pca->i2c,LED0_OFF_L + LED_MULTIPLYER * (led-1));
	}


	if(tmp==-1){
		logString("PCA9685 FAIL : PCA9685_getPWM");
		return -1;
	}
	ledval +=tmp;
	return ledval;
}
