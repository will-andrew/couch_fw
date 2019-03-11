#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "ppm.h"
#include "pins.h"
#include "system.h"

#define TIMEOUT (3000)
#define PRESCALE (64)

#define CODES_TO_US(c) (__builtin_divud(__builtin_muluu((c), PRESCALE), FCY_MHZ))

static uint16_t capture_old[N_CHANNELS] = {0};
static uint16_t ppm_vals[N_CHANNELS][N_ITEMS] = {{0}};
uint16_t ppm_vals_hld[N_CHANNELS][N_ITEMS] = {{0}};
static int ppm_indices[N_CHANNELS] = {-1, -1, -1, -1};

void ppm_init(void)
{
	T2CONbits.TON = 0;
	
	// TMR2 has 64:1 prescaler
	IC5CON1bits.ICTSEL = 0b001;
	IC6CON1bits.ICTSEL = 0b001;
	IC7CON1bits.ICTSEL = 0b001;
	IC8CON1bits.ICTSEL = 0b001;
	
	// Interrupt on every falling edge (0b011 for rising edge)
	IC5CON1bits.ICM = 0b010;
	IC6CON1bits.ICM = 0b010;
	IC7CON1bits.ICM = 0b010;
	IC8CON1bits.ICM = 0b010;
	
	// Free running capture timer
	IC5CON2bits.SYNCSEL = 0;
	IC6CON2bits.SYNCSEL = 0;
	IC7CON2bits.SYNCSEL = 0;
	IC8CON2bits.SYNCSEL = 0;
	
	T2CONbits.TON = 1;
}

// Must be called with same priority as the interrupts
static inline void hold_ppm_vals(int ch)
{
	for (int i = 0; i < N_ITEMS; i++) {
		ppm_vals_hld[ch][i] = CODES_TO_US(ppm_vals[ch][i]);
	}
}

void ppm_pause_interrupts(void)
{
	IEC2bits.IC5IE = 0;
	IEC2bits.IC6IE = 0;
	IEC1bits.IC7IE = 0;
	IEC1bits.IC8IE = 0;
}

void ppm_restart_interrupts(void)
{
	IEC2bits.IC5IE = 1;
	IEC2bits.IC6IE = 1;
	IEC1bits.IC7IE = 1;
	IEC1bits.IC8IE = 1;
}

static inline void process_ic(int channel, uint16_t capture)
{
	volatile uint16_t d = capture - capture_old[channel];
	capture_old[channel] = capture;
	
	if (d > TIMEOUT) {
		// Start of a new sequence
		if (channel == 0) {
			GPIO_3 ^= 1;
		}
		ppm_indices[channel] = 0;
	} else {
		if (ppm_indices[channel] >= 0 && ppm_indices[channel] < N_ITEMS) {
			ppm_vals[channel][ppm_indices[channel]] = d;
			ppm_indices[channel]++;
			GPIO_1 ^= 1;
		}
		if (ppm_indices[channel] == N_ITEMS) {
			GPIO_2 ^= 1;
			hold_ppm_vals(channel);
		}
	}
}

void __attribute__((interrupt, no_auto_psv)) _IC5Interrupt(void)
{
	uint16_t capture = 0;
	while (IC5CON1bits.ICBNE) {
		capture = IC5BUF;
	}
	
	process_ic(0, capture);
	
	IFS2bits.IC5IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _IC6Interrupt(void)
{
	uint16_t capture = 0;
	while (IC6CON1bits.ICBNE) {
		capture = IC6BUF;
	}
	
	process_ic(1, capture);
	
	IFS2bits.IC6IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _IC7Interrupt(void)
{
	uint16_t capture = 0;
	while (IC7CON1bits.ICBNE) {
		capture = IC7BUF;
	}
	
	process_ic(2, capture);
	
	IFS1bits.IC7IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _IC8Interrupt(void)
{
	uint16_t capture = 0;
	while (IC8CON1bits.ICBNE) {
		capture = IC8BUF;
	}
	
	process_ic(3, capture);
	
	IFS1bits.IC8IF = 0;
}