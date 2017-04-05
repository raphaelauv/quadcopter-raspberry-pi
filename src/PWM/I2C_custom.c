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
 * Name        : I2C.cpp
 * Author      : Georgi Todorov
 * Version     :
 * Created on  : Dec 30, 2012
 *
 * Copyright © 2012 Georgi Todorov  <terahz@geodar.com>
 */

#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>      /* Standard I/O functions */
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <syslog.h>		/* Syslog functionality */
#include "I2C_custom.h"

int initI2C_custom(I2C_custom ** i2c_cus,int bus, int address){

	*i2c_cus = (I2C_custom *) malloc(sizeof(I2C_custom));
	if (*i2c_cus == NULL) {
		//logString("MALLOC FAIL : pca");
		return EXIT_FAILURE;
	}

	(*i2c_cus)->_i2cbus = bus;
	(*i2c_cus)->_i2caddr = address;

	snprintf((*i2c_cus)->busfile, sizeof((*i2c_cus)->busfile), "/dev/i2c-%d", bus);
	I2C_custom_openfd(*i2c_cus);
	return 0;
}

void cleanI2C_custom(I2C_custom * i2c_cus) {
	if(i2c_cus!=NULL){
		close(i2c_cus->fd);
		free(i2c_cus);
	}
}
//! Read a single byte from I2C Bus
/*!
 \param address register address to read from
 */
uint8_t I2C_custom_read_byte(I2C_custom * i2c_cus,uint8_t address) {
	if (i2c_cus->fd != -1) {
		uint8_t buff[BUFFER_SIZE];
		buff[0] = address;
		if (write(i2c_cus->fd, buff, BUFFER_SIZE) != BUFFER_SIZE) {
			syslog(LOG_ERR,
					"I2C slave 0x%x failed to go to register 0x%x [read_byte():write %d]",
					i2c_cus->_i2caddr, address, errno);
			return (-1);
		} else {
			if (read(i2c_cus->fd, i2c_cus->dataBuffer, BUFFER_SIZE) != BUFFER_SIZE) {
				syslog(LOG_ERR,
						"Could not read from I2C slave 0x%x, register 0x%x [read_byte():read %d]",
						i2c_cus->_i2caddr, address, errno);
				return (-1);
			} else {
				return i2c_cus->dataBuffer[0];
			}
		}
	} else {
		syslog(LOG_ERR, "Device File not available. Aborting read");
		return (-1);
	}

}
//! Write a single byte from a I2C Device
/*!
 \param address register address to write to
 \param data 8 bit data to write
 */
uint8_t I2C_custom_write_byte(I2C_custom * i2c_cus,uint8_t address, uint8_t data) {
	if (i2c_cus->fd != -1) {
		uint8_t buff[2];
		buff[0] = address;
		buff[1] = data;
		if (write(i2c_cus->fd, buff, sizeof(buff)) != 2) {
			syslog(LOG_ERR,
					"Failed to write to I2C Slave 0x%x @ register 0x%x [write_byte():write %d]",
					i2c_cus->_i2caddr, address, errno);
			return (-1);
		} else {
			syslog(LOG_INFO, "Wrote to I2C Slave 0x%x @ register 0x%x [0x%x]",
					i2c_cus->_i2caddr, address, data);
			return (-1);
		}
	} else {
		syslog(LOG_INFO, "Device File not available. Aborting write");
		return (-1);
	}
	return 0;
}
//! Open device file for I2C Device
int I2C_custom_openfd(I2C_custom * i2c_cus) {
	if ((i2c_cus->fd = open(i2c_cus->busfile, O_RDWR)) < 0) {
		syslog(LOG_ERR, "Couldn't open I2C Bus %d [openfd():open %d]", i2c_cus->_i2cbus,
				errno);
		return -1;
	}
	if (ioctl(i2c_cus->fd, I2C_SLAVE, i2c_cus->_i2caddr) < 0) {
		syslog(LOG_ERR, "I2C slave %d failed [openfd():ioctl %d]", i2c_cus->_i2caddr,
				errno);
		return -1;
	}
	return 0;
}
