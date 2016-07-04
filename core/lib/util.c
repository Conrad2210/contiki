/*
 * util.c
 *
 *  Created on: Jul 4, 2016
 *      Author: user
 */

#include "util.h"
#include <math.h>

float calcDistance(float txPowerInMW, float rxPowerInMW) {

	float lambda = SPEED_OF_LIGHT / 2.4e9;
	float temp1 = txPowerInMW * pow(lambda, 2);
	float temp2 = (16 * pow(3.14, 2) * rxPowerInMW);
	float temp3 = temp1 / temp2;
	float temp4 = 1.0 / 3.0;

	return pow(temp3, temp4);
}

float mWTodBm(float mW) {
	return 10 * log10(mW);
}

float dBmTomW(float dBm) {
	return pow(10, dBm / 10);
}
