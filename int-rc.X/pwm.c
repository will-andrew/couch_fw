#include <xc.h>
#include <stdint.h>

#include "system.h"
#include "pins.h"
#include "util.h"
#include "pwm.h"

#define US_TO_CODES(x) (__builtin_muluu(x, FCY_MHZ) / PRESCALE)

#define PWM_FREQ (100)
#define PWM_MIN_LEN US_TO_CODES(1000)
#define PWM_MAX_LEN US_TO_CODES(2000)

#define PRESCALE (64)

void pwm_init(void)
{
	// Setup OC1,2,3,4,5 synchronised
	// Outputs of OCs not used directly. OC1,2,3,4 interrupts used to clear
	// output, OC5 permanently has 0 duty and is used to set output
	// Timer2 used as 70/64 MHz timebase for OCs
	
	OC1CON1 = OC2CON1 = OC3CON1 = OC4CON1 = 0;
	OC1CON2 = OC2CON2 = OC3CON2 = OC4CON2 = 0;
	
	OC1CON1bits.OCTSEL = 0b000; // TMR2 clock source
	OC2CON1bits.OCTSEL = 0b000; // TMR2 clock source
	OC3CON1bits.OCTSEL = 0b000; // TMR2 clock source
	OC4CON1bits.OCTSEL = 0b000; // TMR2 clock source
	OC5CON1bits.OCTSEL = 0b000; // TMR2 clock source
	
	OC1R = OC2R = OC3R = OC4R = US_TO_CODES(1500);
	OC5R = 0;
	
	OC1RS = FCY / PRESCALE / PWM_FREQ; // OC1 is the period trigger for the others
	OC1CON2bits.SYNCSEL = 0b111111; // OC1RS compare is reset trigger
	OC2CON2bits.SYNCSEL = 0b000001; // OC1 is the trigger
	OC3CON2bits.SYNCSEL = 0b000001; // OC1 is the trigger
	OC4CON2bits.SYNCSEL = 0b000001; // OC1 is the trigger
	OC5CON2bits.SYNCSEL = 0b000001; // OC1 is the trigger
	
	OC1CON1bits.OCM = 3; // Edge aligned PWM
	OC2CON1bits.OCM = 3; // Edge aligned PWM
	OC3CON1bits.OCM = 3; // Edge aligned PWM
	OC4CON1bits.OCM = 3; // Edge aligned PWM
	OC5CON1bits.OCM = 3; // Edge aligned PWM
}

void pwm_set_value(int index, uint16_t value)
{
	LIMIT(value, PWM_MIN_LEN, PWM_MAX_LEN);
	
	value = US_TO_CODES(value);
	
	switch (index) {
		case 0:
			OC1R = value;
			break;
		case 1:
			OC2R = value;
			break;
		case 2:
			OC3R = value;
			break;
		case 3:
			OC4R = value;
			break;
	}
}

void pwm_set_values(uint16_t *vals)
{
	uint16_t val1 = US_TO_CODES(vals[0]);
	uint16_t val2 = US_TO_CODES(vals[1]);
	uint16_t val3 = US_TO_CODES(vals[2]);
	uint16_t val4 = US_TO_CODES(vals[3]);
	
	LIMIT(val1, PWM_MIN_LEN, PWM_MAX_LEN);
	LIMIT(val2, PWM_MIN_LEN, PWM_MAX_LEN);
	LIMIT(val3, PWM_MIN_LEN, PWM_MAX_LEN);
	LIMIT(val4, PWM_MIN_LEN, PWM_MAX_LEN);
	
	OC1R = val1;
	OC2R = val2;
	OC3R = val3;
	OC4R = val4;
}

void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void)
{
	M1RC = 0;
	IFS0bits.OC1IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _OC2Interrupt(void)
{
	M2RC = 0;
	IFS0bits.OC2IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _OC3Interrupt(void)
{
	M3RC = 0;
	IFS1bits.OC3IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _OC4Interrupt(void)
{
	M4RC = 0;
	IFS1bits.OC4IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _OC5Interrupt(void)
{
	M1RC = 1;
	M2RC = 1;
	M3RC = 1;
	M4RC = 1;
	IFS2bits.OC5IF = 0;
}