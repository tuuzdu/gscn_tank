/*************************************************** 
  This is a library for our Adafruit 16-channel PWM & Servo driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to  
  interface.

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "driver_PCA9685.h"
#include <iostream>

/**************************************************************************/
/*! 
    @brief  Instantiates a new PCA9685 PWM driver chip with the I2C address on the Wire interface. On Due we use Wire1 since its the interface on the 'default' I2C pins.
    @param  addr The 7-bit I2C address to locate this chip, default is 0x40
*/
/**************************************************************************/
PCA9685Driver::PCA9685Driver(char bus, char addr) {
  _i2caddr = addr;
  _i2c = new I2C(bus, addr);
}

/**************************************************************************/
/*! 
    @brief  Setups the I2C interface and hardware
*/
/**************************************************************************/
void PCA9685Driver::begin(void) {
  reset();
  // set a default frequency
  setPWMFreq(60);
}


/**************************************************************************/
/*! 
    @brief  Sends a reset command to the PCA9685 chip over I2C
*/
/**************************************************************************/
void PCA9685Driver::reset(void) {
  write8(PCA9685_MODE1, 0x80);
  usleep(10);
}

/**************************************************************************/
/*! 
    @brief  Sets the PWM frequency for the entire chip, up to ~1.6 KHz
    @param  freq Floating point frequency that we will attempt to match
*/
/**************************************************************************/
void PCA9685Driver::setPWMFreq(float freq) {
  // std::cout << "Attempting to set freq " << freq << std::endl;

  freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;

  // std::cout << "Estimated pre-scale: " << (int)prescaleval << std::endl;
  char prescale = floor(prescaleval + 0.5);
  // std::cout << "Final pre-scale: " << (int)prescale << std::endl;
  
  char oldmode = read8(PCA9685_MODE1);
  char newmode = (oldmode&0x7F) | 0x10; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep
  write8(PCA9685_PRESCALE, prescale); // set the prescaler
  write8(PCA9685_MODE1, oldmode);
  usleep(5);
  write8(PCA9685_MODE1, oldmode | 0xa0);  //  This sets the MODE1 register to turn on auto increment.

  // std::cout << "PCA9685_MODE1 " << PCA9685_MODE1 << std::endl;
  // std::cout << "Mode now " << std::hex << read8(PCA9685_MODE1) << std::endl;
}

/**************************************************************************/
/*! 
    @brief  Sets the PWM output of one of the PCA9685 pins
    @param  num One of the PWM output pins, from 0 to 15
    @param  on At what point in the 4096-part cycle to turn the PWM output ON
    @param  off At what point in the 4096-part cycle to turn the PWM output OFF
*/
/**************************************************************************/
void PCA9685Driver::setPWM(char num, uint on, uint off) {
  char addr = LED0_ON_L+4*num;
  // std::cout << "Channel addr 0x" << std::hex << (int)addr << std::endl;
  // std::cout << "Channel on " << on << std::endl;
  // std::cout << "Channel on " << (on>>8) << std::endl;
  // std::cout << "Channel off " << off << std::endl;
  // std::cout << "Channel off " << (off>>8) << std::endl;
  _i2c->writeI2CDeviceByte((char)addr, (char)on);
  _i2c->writeI2CDeviceByte((char)(addr + 1), (char)(on>>8));
  _i2c->writeI2CDeviceByte((char)(addr + 2), (char)off);
  _i2c->writeI2CDeviceByte((char)(addr + 3), (char)(off>>8));

}

/**************************************************************************/
/*! 
    @brief  Helper to set pin PWM output. Sets pin without having to deal with on/off tick placement and properly handles a zero value as completely off and 4095 as completely on.  Optional invert parameter supports inverting the pulse for sinking to ground.
    @param  num One of the PWM output pins, from 0 to 15
    @param  val The number of ticks out of 4096 to be active, should be a value from 0 to 4095 inclusive.
    @param  invert If true, inverts the output, defaults to 'false'
*/
/**************************************************************************/
void PCA9685Driver::setPin(char num, uint val, bool invert)
{
  // Clamp value between 0 and 4095 inclusive.
  val = (val<4095 ? val : (uint)4095);
  if (invert) {
    if (val == 0) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == 4095) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, 4095-val);
    }
  }
  else {
    if (val == 4095) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == 0) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, val);
    }
  }
}

/*******************************************************************************************/

char PCA9685Driver::read8(char addr) {
  char d = _i2c->readI2CDeviceByte(addr);
  // std::cout << "Read addr 0x" << std::hex << (int)addr << " data 0x" << std::hex << (int)d << std::endl;
  return  d;
}

void PCA9685Driver::write8(char addr, char d) {
  // std::cout << "Write addr 0x" << std::hex << (int)addr << " data 0x" << std::hex << (int)d << std::endl;
  _i2c->writeI2CDeviceByte(addr, d);
}
