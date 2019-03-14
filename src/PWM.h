/*
 * PWM.h
 *
 *  Created on: 05.03.2019
 *      Author: tuuzdu
 */

#ifndef PWM_H_
#define PWM_H_

class PWM {
	public:
		PWM();
		~PWM();
		void ChCtl(uint ch0, uint ch1, uint ch2, uint ch3);
		void WheelNorm(float left, float right);

	private:
		PCA9685Driver* _driver;

};

float constrain(float val, float min, float max)
{
	return (val < min) ? min : ((val > max) ? max : val);
}

#endif /* PWM_H_ */
