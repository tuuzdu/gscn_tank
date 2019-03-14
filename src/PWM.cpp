/*
 * PWM.cpp
 *
 *  Created on: 05.03.2019
 *      Author: tuuzdu
 */

#include "driver_PCA9685.h"
#include "PWM.h"
#include <ros/ros.h>

PWM::PWM(){
	_driver = new PCA9685Driver(1, 0x40);
	_driver->begin();
	ChCtl(0, 0, 0, 0);
}

PWM::~PWM(){
	ChCtl(0, 0, 0, 0);
}

void PWM::ChCtl(uint ch0, uint ch1, uint ch2, uint ch3){
	_driver->setPin(0, ch0);
	_driver->setPin(1, ch1);
	_driver->setPin(2, ch2);
	_driver->setPin(3, ch3);

}

#define DUTY_CYCLE 	4095
#define MAX_ANGULAR 1000
#define MAX_LINEAR 	DUTY_CYCLE-MAX_ANGULAR

void PWM::WheelNorm(float left, float right){
	left = constrain(left, -1, 1);
	right = constrain(right, -1, 1);
	uint ch0 = 0, ch1 = 0, ch2 = 0, ch3 = 0;

	if (left > 0){
		ch1 = DUTY_CYCLE * left;
	}
	else if (left < 0){
		ch0 = DUTY_CYCLE * (-left);
	}
	else if (left == 0){
		ch0 = 0;
		ch1 = 0;
	}
	if (right > 0){
		ch3 = DUTY_CYCLE * right;
	}
	else if (right < 0){
		ch2 = DUTY_CYCLE * (-right);
	}
	else if (right == 0){
		ch2 = 0;
		ch3 = 0;
	}

	ROS_INFO ("ch0 %u ch1 %u ch2 %u ch3 %u", ch0, ch1, ch2, ch3);

	ChCtl(ch0, ch1, ch2, ch3);
}