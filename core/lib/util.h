/*
 * util.h
 *
 *  Created on: Jul 4, 2016
 *      Author: user
 */

#ifndef DEWI_NIMBUS_CONTIKI_CORE_LIB_UTIL_H_
#define DEWI_NIMBUS_CONTIKI_CORE_LIB_UTIL_H_


#ifndef SPEED_OF_LIGHT
	#define SPEED_OF_LIGHT 299792458
#endif

float calcDistance(float txPowerInMW, float rxPowerInMW);

float mWTodBm(float mW);

float dBmTomW(float dBm);

#endif /* DEWI_NIMBUS_CONTIKI_CORE_LIB_UTIL_H_ */
