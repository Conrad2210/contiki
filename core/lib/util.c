/*
 * util.c
 *
 *  Created on: Jul 4, 2016
 *      Author: user
 */

#include "util.h"
#include <math.h>

float calcDistance(float txPowerInMW, float rxPowerInMW) {

	printf("txPowerInMW: %d \n",txPowerInMW);
	printf("rxPowerInMW: %d \n",rxPowerInMW);
	float lambda = SPEED_OF_LIGHT / 2.4e9;

	printf("lambda: %d \n",lambda);
	float temp1 = txPowerInMW * pow(lambda, 2);
	printf("lambda: %d \n",lambda);
	float temp2 = (16 * pow(3.14, 2) * rxPowerInMW);
	float temp3 = temp1 / temp2;
	float temp4 = 1.0 / 3.0;
	float distance = pow(temp3, temp4);
	printf("Distance: %d m\n",distance);
	return distance;
}

float mWTodBm(float mW) {
	return 10 * log10(mW);
}

float dBmTomW(float dBm) {
	return pow(10, dBm / 10);
}

#include<stdio.h>
#include<math.h>

// reverses a string 'str' of length 'len'
void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

 // Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
    	printf(" str[i++] = (x%10) + '0'; \n");
        str[i++] = (x%10) + '0';
    	printf("    x = x/10; \n");
    	   x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

	printf(" reverse(str, i); \n");
    reverse(str, i);
	printf(" str[i] = '\0'; \n");
    str[i] = '\0';
    return i;
}

// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
	printf("int ipart = (int)n; \n");
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
	printf(" float fpart = n - (float)ipart;; \n");
    float fpart = n - (float)ipart;

    // convert integer part to string
	printf(" int i = intToStr(ipart, res, 0); \n");
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
    	printf("res[i] = '.';  // add dot \n");
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
    	printf(" fpart = fpart * pow(10, afterpoint); \n");
        fpart = fpart * pow(10, afterpoint);

    	printf("rintToStr((int)fpart, res + i + 1, afterpoint); \n");
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}
