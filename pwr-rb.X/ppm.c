#include <xc.h>
#include <stdint.h>

#include "ppm.h"
#include "system.h"
#include "drive.h"
#include "pins.h"

#define MIN_PPM_TIME (100)
#define MAX_PPM_TIME (4000)
#define PPM_ON_TIME (50)

#define FCY_MHZ (FCY / 1000000)

uint16_t ppm_pulses[PPM_ITEMS]; // Current, temperature, voltage, error, drive value

static int pulse_index = 0;
static uint16_t pwm_start = 0;

uint16_t cmd_duration = 0;

void ppm_init(void)
{
	OC1CON1bits.OCTSEL = 0b111; // FP is the clock source
	OC1R = PPM_ON_TIME * FCY_MHZ;
	
	// Off time
	T3CONbits.TCKPS = 0b01; // 1:8 prescale
	T3CONbits.TON = 0;
	
	IC1CON1bits.ICTSEL = 0b111;
	IC1CON1bits.ICI = 0; // Interrupt on every capture
	IC1CON2bits.SYNCSEL = 0; // Free running	
	IC1CON1bits.ICM = 0b001; // Rising edge
}

void __attribute__((interrupt, no_auto_psv)) _IC1Interrupt(void)
{
	if (RC_IN == 0) {
		// Rising (inverted due to opto)
		while (IC1CON1bits.ICBNE) {
			pwm_start = IC1BUF;
		}
	} else {
		// Falling
		uint16_t pwm_end = IC1BUF;
		while (IC1CON1bits.ICBNE) {
			pwm_end = IC1BUF;
		}

		cmd_duration = (pwm_end - pwm_start) / FCY_MHZ;

		drive_set(cmd_duration - 1500);
	}
	
	IFS0bits.IC1IF = 0;
}

void ppm_set(uint8_t index, uint16_t val)
{
	if (index < PPM_ITEMS) {
		if (val < MIN_PPM_TIME) {
			val = MIN_PPM_TIME;
		} else if (val > MAX_PPM_TIME) {
			val = MAX_PPM_TIME;
		}

		ppm_pulses[index] = val;
	}
}

static void output_pulse(void)
{
	LATBbits.LATB5 ^= 1;
	T3CONbits.TON = 0;
		
	if (pulse_index < PPM_ITEMS) {
		OC1CON1bits.OCM = 0;
		OC1TMR = 0;
		TMR3 = 0;
		PR3 = (FCY_MHZ / 8) * ppm_pulses[pulse_index++];
		T3CONbits.TON = 1;
		OC1CON1bits.OCM = 0b010; // Single shot high pulse
	} else if (pulse_index == PPM_ITEMS) {
		OC1CON1bits.OCM = 0;
		OC1TMR = 0;
		OC1CON1bits.OCM = 0b010; // Final pulse
	}
}

void ppm_output_train(void)
{
	pulse_index = 0;
	output_pulse();
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void)
{
	output_pulse();	
	IFS0bits.T3IF = 0;
}
