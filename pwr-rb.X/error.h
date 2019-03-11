/* 
 * File:   error.h
 * Author: William Andrew
 *
 * Created on 26 August 2017, 2:58 PM
 */

#ifndef ERROR_H
#define	ERROR_H

#include <stdint.h>

#define PEAK_CURRENT_TRIP_AMPS (50)
#define AVE_CURRENT_TRIP_AMPS (20)
#define LOW_VOLTAGE_TRIP_VOLTS (20)
#define HIGH_VOLTAGE_TRIP_VOLTS (70)
#define HIGH_TEMPERATURE_TRIP_DEGC (85)

enum errors {
	ERROR_NONE = 0,
	ERROR_PEAK_CURRENT = 1,
	ERROR_AVE_CURRENT = 2,
	ERROR_HIGH_VOLTAGE = 3,
	ERROR_LOW_VOLTAGE = 4,
	ERROR_HIGH_TEMP = 5,
	N_ERRORS
};

void error_set(uint8_t error, int16_t value);
uint8_t error_get(void);
void error_print_info(void);

#endif	/* ERROR_H */

