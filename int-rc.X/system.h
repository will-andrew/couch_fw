/* 
 * File:   system.h
 * Author: will
 *
 * Created on 5 April 2015, 5:35 PM
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#include <stdint.h>

#define FOSC (140000000ULL)
#define FCY (FOSC / 2)
#define FCY_MHZ ((uint16_t)(FCY / 1000000))

#define GetSystemClock()		(FOSC)			// Hz
#define GetInstructionClock()	(GetSystemClock()/2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Might need changing if using Doze modes.
#define GetPeripheralClock()	(GetSystemClock()/2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Divisor may be different if using a PIC32 since it's configurable.

#define HIGH_WORD(x) ((x) >> 16)
#define LOW_WORD(x) ((x) & 65535)

#define ADC1_SAMPLES_BITS 2
#define ADC1_SAMPLES (1 << ADC1_SAMPLES_BITS)

/*
 #define AN_VBAT 2 // RB0
#define AN_TH1 1 // RA1
#define AN_TH2 0 // RA0
#define AN_TH3 9 // RA11
#define AN_TH4 10 // RA12
#define AN_ESTOP 8 // RC2
#define AN_ISNS1 16 // RG9
#define AN_ISNS2 17 // RG8
#define AN_ISNS3 18 // RG7
#define AN_ISNS4 19 // RG6
#define AN_VREF 3 //RB1*/

struct adc1_buf {
    uint16_t dummy1[9 * ADC1_SAMPLES];
	uint16_t estop[ADC1_SAMPLES];
    uint16_t vbat[ADC1_SAMPLES];
	uint16_t dummy2[2 * ADC1_SAMPLES];
	uint16_t th2[ADC1_SAMPLES];
	uint16_t th3[ADC1_SAMPLES];
	uint16_t th4[ADC1_SAMPLES];
	uint16_t dummy3[8 * ADC1_SAMPLES];
    uint16_t th1[ADC1_SAMPLES];
	uint16_t dummy4[7 * ADC1_SAMPLES];
};
extern struct adc1_buf adc1_buf;

extern uint16_t current_offsets[4];

#define ADC1_CHS_BITFIELD 0b00000000100000001110011000000000ULL

void system_init(void);
void interrupt_init(void);

uint16_t sum_samples(uint16_t *buf);

#endif	/* SYSTEM_H */

