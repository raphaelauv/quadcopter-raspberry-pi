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
 * Name        		: I2C_custom.c
 * Author      		: raphaelauv
 * Original Author  : Georgi Todorov <terahz@geodar.com>
 * Created on  		: Apr 3, 2017
 *
 * Copyright © 2017 Raphaelauv
 */

#ifndef I2C_custom_H_
#define I2C_custom_H_
#include <inttypes.h>
#include "../log.h"

#define BUFFER_SIZE 0x01  //1 byte buffer

typedef struct I2C_custom {
	int _i2caddr;
	int _i2cbus;
	char busfile[64];
	int fd;
	uint8_t dataBuffer[BUFFER_SIZE];
} I2C_custom;
	

int initI2C_custom(I2C_custom ** i2c_cus,int bus, int address);
void cleanI2C_custom(I2C_custom * i2c_cus);
int I2C_custom_openfd(I2C_custom * i2c_cus);

uint8_t I2C_custom_read_byte(I2C_custom * i2c_cus,uint8_t);
uint8_t I2C_custom_write_byte(I2C_custom * i2c_cus,uint8_t, uint8_t);


#endif /* I2C_custom_H_ */