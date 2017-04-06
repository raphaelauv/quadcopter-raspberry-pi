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

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>      /* Standard I/O functions */
#include <fcntl.h>
#include <syslog.h>		/* Syslog functionallity */
#include <inttypes.h>
#include <errno.h>
#include <math.h>

#include "PCA9685.h"

//! Constructor takes bus and address arguments
/*!
 \param bus the bus to use in /dev/i2c-%d.
 \param address the device address on bus
 */
int initPCA9685(PCA9685 ** pca,int bus, int address){

	*pca = (PCA9685 *) malloc(sizeof(PCA9685));
	if (*pca == NULL) {
		//logString("MALLOC FAIL : PCA9685");
		return -1;
	}

	I2C_custom * i2c_c;
	if(initI2C_custom(&i2c_c,bus,address)){
		return -1;
	}
	(*pca)->i2c =i2c_c;
	if(PCA9685_reset((*pca))){
		return -1;
	}
	if(PCA9685_setPWMFreq((*pca),1000)){
		return -1;	
	}
}

void cleanPCA9685(PCA9685 *pca){
	if(pca!=NULL){
		cleanI2C_custom(pca->i2c);
	}
	free(pca);
}

//! Sets PCA9685 mode to 00
int PCA9685_reset(PCA9685 *pca) {
		int result=0;
		result+=I2C_custom_write_byte(pca->i2c,MODE1, 0x00); //Normal mode
		result+=I2C_custom_write_byte(pca->i2c,MODE2, 0x04); //totem pole
		if(result){
			//logString("PCA9685 FAIL : PCA9685_reset");
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
		result+=I2C_custom_write_byte(pca->i2c,MODE1, 0x10); //sleep
		result+=I2C_custom_write_byte(pca->i2c,PRE_SCALE, prescale_val); // multiplyer for PWM frequency
		result+=I2C_custom_write_byte(pca->i2c,MODE1, 0x80); //restart
		result+=I2C_custom_write_byte(pca->i2c,MODE2, 0x04); //totem pole (default)

		if(result){
			//logString("PCA9685 FAIL : PCA9685_setPWMFreq");
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
		result+=I2C_custom_write_byte(pca->i2c,LED0_ON_L + LED_MULTIPLYER * (led - 1), on_value & 0xFF);
		result+=I2C_custom_write_byte(pca->i2c,LED0_ON_H + LED_MULTIPLYER * (led - 1), on_value >> 8);
		result+=I2C_custom_write_byte(pca->i2c,LED0_OFF_L + LED_MULTIPLYER * (led - 1), off_value & 0xFF);
		result+=I2C_custom_write_byte(pca->i2c,LED0_OFF_H + LED_MULTIPLYER * (led - 1), off_value >> 8);
		if(result){
			//logString("PCA9685 FAIL : PCA9685_setPWM_2");
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
	ledval = I2C_custom_read_byte(pca->i2c,LED0_OFF_H + LED_MULTIPLYER * (led-1));
	if(ledval==-1){
		//logString("PCA9685 FAIL : PCA9685_getPWM");
		return -1;
	}
	ledval = ledval & 0xf;
	ledval <<= 8;
	int tmp =I2C_custom_read_byte(pca->i2c,LED0_OFF_L + LED_MULTIPLYER * (led-1));
	if(tmp==-1){
		//logString("PCA9685 FAIL : PCA9685_getPWM");
		return -1;
	}
	ledval +=tmp;
	return ledval;
}
