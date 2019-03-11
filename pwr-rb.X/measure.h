/* 
 * File:   measure.h
 * Author: William Andrew
 *
 * Created on 26 August 2017, 2:54 PM
 */

#ifndef MEASURE_H
#define	MEASURE_H

#include <stdint.h>
#include "util.h"

#define VOLTS_TO_CODES(v) ((v) * 10240UL * 33 / 853 / 33)
#define AMPS_TO_CODES(a) ((a) * 10240L * 50 / 1000 / 33) // INA240A2 = 50x gain, 1 mohm resistor
#define CODES_TO_VOLTS(c) ((c) / 12) // Not exact
#define CODES_TO_AMPS(c) ((c) * 33 / 512)

#define ADC1_SAMPLES_BITS (4)
#define ADC1_SAMPLES (1 << ADC1_SAMPLES_BITS)

struct adc1_buf {
	uint16_t vref[ADC1_SAMPLES];
    uint16_t isns[ADC1_SAMPLES];
	uint16_t vsns[ADC1_SAMPLES];
	uint16_t th1[ADC1_SAMPLES];
	uint16_t th2[ADC1_SAMPLES];
    uint16_t dummy3[11 * ADC1_SAMPLES];
};
STATIC_ASSERT(sizeof(struct adc1_buf) == ADC1_SAMPLES * 16 * 2, ADC1_BUF_SIZE_BAD);

extern struct adc1_buf adc1_buf;

void measure_init(void);
void measure_loop(void); // Call at ~100 Hz
void measure_slow_loop(void); // Call at ~1 Hz
void measure_print_info(void);

uint16_t measure_voltage(void);
int16_t measure_current(void);
int8_t measure_temperature_degc(void);

#endif	/* MEASURE_H */

