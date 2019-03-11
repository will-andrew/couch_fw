/* 
 * File:   thermistor.h
 * Author: will
 *
 * Created on 16 August 2017, 10:11 PM
 */

#ifndef THERMISTOR_H
#define	THERMISTOR_H

#include <stdint.h>

#define INVALID_TEMP (-128)

int8_t thermistor_lookup(uint16_t code);

#endif	/* THERMISTOR_H */

