/*
 * I2C.cpp
 *
 *  Created on: 23.08.2013
 *      Author: tuuzdu
 */
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include "I2C.h"
#include <iostream>

#define MAX_BUS 64

using namespace std;

I2C::I2C(int bus, int address) {
	I2CBus = bus;
	I2CAddress = address;
}

int I2C::writeI2CDeviceByte(char address, char value) {

	// cout << "Starting I2C sensor state write" << endl;
	// cout << "BUS " << I2CAddress << endl;
	char namebuf[MAX_BUS];
	snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);
	int file;
	if ((file = open(namebuf, O_RDWR)) < 0) {
		cout << "Failed to open Sensor on " << namebuf << " I2C Bus" << endl;
		return (1);
	}
	if (ioctl(file, I2C_SLAVE, I2CAddress) < 0) {
		cout << "I2C_SLAVE address " << I2CAddress << " failed..." << endl;
		return (2);
	}

	char buffer[2];
	buffer[0] = address;
	buffer[1] = value;
	if (write(file, buffer, 2) != 2) {
		cout << "Failure to write values to I2C Device address." << endl;
		return (3);
	}
	close(file);
//   cout << "Finished I2C sensor state write" << endl;
	return 0;
}

char I2C::readI2CDeviceByte(char address) {

	//  cout << "Starting BMA180 I2C sensor state byte read" << endl;
	char namebuf[MAX_BUS];
	snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);
	int file;
	if ((file = open(namebuf, O_RDWR)) < 0) {
		cout << "Failed to open Sensor on " << namebuf << " I2C Bus" << endl;
		return (1);
	}
	if (ioctl(file, I2C_SLAVE, I2CAddress) < 0) {
		cout << "I2C_SLAVE address " << I2CAddress << " failed..." << endl;
		return (2);
	}

	char buf[1];
	buf[0] = address;
	if (write(file, buf, 1) != 1) {
		cout << "Failed to Reset Address in readFullSensorState() " << endl;
	}

	unsigned char buffer[1];
	buffer[0] = address;
	if (read(file, buffer, 2) != 2) {
		cout << "Failure to read value from I2C Device address." << endl;
	}
	close(file);
	//cout << (int) buffer [0] << endl;
	return buffer[0];
}
