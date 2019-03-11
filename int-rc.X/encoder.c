#include <xc.h>
#include <stdint.h>

#include "pins.h"

// Encoders 2 and 4 use the PIC's QEI modules
// Encoders 1 and 3 use software (encoder-ic.s)

uint16_t old_enc1; // State for QE1
uint16_t old_enc3; // State for QE3
int32_t disp1;
int32_t disp3;

int encoder_failed = 0;

#define TICKS_PER_REV 1024 // Must be a power of 2

int32_t round_ticks(int32_t x)
{
	if (x < 0) {
		x -= TICKS_PER_REV / 2;
	} else {
		x += TICKS_PER_REV / 2;
	}
	
	return x & ~(TICKS_PER_REV - 1);
}

void encoder_init(void)
{
	// Setup IC1-4 as rising edge interrupts
	IC1CON1bits.ICM = 0b001;
	IC2CON1bits.ICM = 0b001;
	IC3CON1bits.ICM = 0b001;
	IC4CON1bits.ICM = 0b001;
	
	// QEI (wheels)
	QEI1CONbits.CCM = 0; // 4x counting, no reset
	QEI2CONbits.CCM = 0;
	QEI1CONbits.PIMOD = 0; // Index pulse is ignored
	QEI2CONbits.PIMOD = 0;
	POS1CNTL = 0;
	POS2CNTL = 0;
	// TODO swap inputs if necessary using QEIxIOCbits.SWPAB
	QEI1CONbits.QEIEN = 1;
	QEI2CONbits.QEIEN = 1;
	
	// Setup watchdog timers for software encoders to stop unbounded CPU use in
	// the case of a faulty encoder or vibrations causing a high speed
	// oscillation on the interrupt pins
	// T6 is the first timer that is cleared when the encoder interrupts occur.
	// T6 needs to be able to expire in normal operation and not if pulses are
	// too fast. Max motor rpm = 600 rpm * 7.5 gear ratio * 1024 ticks per rev
	// = 461 kHz
	T6CONbits.TCKPS = 0; // 1:1 prescale
	PR6 = 130; // 538 kHz
	T7CONbits.TCKPS = 0b10; // 1:64 prescale
	T6CONbits.TON = 1;
	T7CONbits.TON = 1;
}

// T6 interrupt in encoder-ic.s

void __attribute__((interrupt, no_auto_psv)) _T7Interrupt(void)
{
	// Encoder(s) have failed and are producing too many interrupts
	encoder_failed = 1;
	
	// Disable encoder interrupts
	IEC2bits.IC3IE = 0;
	IEC2bits.IC4IE = 0;
	IEC0bits.IC1IE = 0;
	IEC0bits.IC2IE = 0;
	IEC2bits.T6IE = 0;
	IEC3bits.T7IE = 0;
	IEC1bits.CNIE = 0;
	
	IFS3bits.T7IF = 0;
}

void encoder_readall(int32_t *disps)
{
	disps[0] = disp1 * 2;
	disps[1] = POS1CNTL;
	disps[2] = disp3 * 2;
	disps[3] = POS2CNTL;
	disps[1] |= ((int32_t)POS1HLD << 16);
	disps[3] |= ((int32_t)POS2HLD << 16);
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
	// Index pulse interrupt
	if (QEI1) {
		disp1 = round_ticks(disp1);
	}
	if (QEI2) {
		int32_t disp2 = POS1CNTL;
		disp2 |= ((int32_t)POS1HLD << 16);
		disp2 = round_ticks(disp2);
		POS1HLD = disp2 >> 16;
		POS1CNTL = disp2;
	}
	if (QEI3) {
		disp3 = round_ticks(disp3);
	}
	if (QEI4) {
		int32_t disp4 = POS2CNTL;
		disp4 |= ((int32_t)POS2HLD << 16);
		disp4 = round_ticks(disp4);
		POS2HLD = disp4 >> 16;
		POS2CNTL = disp4;
	}
	
	IFS1bits.CNIF = 0;
}