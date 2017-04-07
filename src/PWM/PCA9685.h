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
 * Name        		: PCA9685.h
 * Author      		: raphaelauv
 * Original Author  : Georgi Todorov <terahz@geodar.com>
 * Created on  		: Apr 3, 2017
 *
 * Copyright © 2017 Raphaelauv
 */

#ifndef _PCA9685_H
#define _PCA9685_H
#include <inttypes.h>
#include "I2C_custom.h"

#ifdef __arm__
#include <wiringPi.h>
#include <wiringPiI2C.h>
#endif

#include "../log.h"

// Register Definitions
#define MODE1 0x00			//Mode  register  1
#define MODE2 0x01			//Mode  register  2
#define SUBADR1 0x02		//I2C-bus subaddress 1
#define SUBADR2 0x03		//I2C-bus subaddress 2
#define SUBADR3 0x04		//I2C-bus subaddress 3
#define ALLCALLADR 0x05     //LED All Call I2C-bus address
#define LED0 0x6			//LED0 start register
#define LED0_ON_L 0x6		//LED0 output and brightness control byte 0
#define LED0_ON_H 0x7		//LED0 output and brightness control byte 1
#define LED0_OFF_L 0x8		//LED0 output and brightness control byte 2
#define LED0_OFF_H 0x9		//LED0 output and brightness control byte 3
#define LED_MULTIPLYER 4	// For the other 15 channels
#define ALLLED_ON_L 0xFA    //load all the LEDn_ON registers, byte 0 (turn 0-7 channels on)
#define ALLLED_ON_H 0xFB	//load all the LEDn_ON registers, byte 1 (turn 8-15 channels on)
#define ALLLED_OFF_L 0xFC	//load all the LEDn_OFF registers, byte 0 (turn 0-7 channels off)
#define ALLLED_OFF_H 0xFD	//load all the LEDn_OFF registers, byte 1 (turn 8-15 channels off)
#define PRE_SCALE 0xFE		//prescaler for output frequency
#define CLOCK_FREQ 25000000.0 //25MHz default osc clock

#define CHANNEL_I2C 1

#define MINIMUM_LED_VALUE 1

#define PCA9685_FREQUENCY 230

typedef struct PCA9685 {
	I2C_custom *i2c;
	int WiringPiI2C_fd;
} PCA9685;
	

int initPCA9685(PCA9685 ** pca,int bus, int address);
void cleanPCA9685(PCA9685 *pca);

int PCA9685_reset(PCA9685 *pca);

int PCA9685_setPWMFreq(PCA9685 *pca,int freq);
int PCA9685_setPWM_1(PCA9685 *pca,uint8_t led, int value);
int PCA9685_setPWM_2(PCA9685 *pca,uint8_t led, int on_value, int off_value);
int PCA9685_getPWM(PCA9685 *pca,uint8_t led);

#endif
