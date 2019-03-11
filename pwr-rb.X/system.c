#include <xc.h>

#include "pins.h"
#include "system.h"
#include "drive.h"

void system_init(void)
{
	// Oscillator
	CLKDIVbits.PLLPRE = 2;					// Divide by 4, 4 MHz
	PLLFBD = 62;							// Multiply by 64, 256 MHz
	CLKDIVbits.PLLPOST = 0b01;					// Divide by 4, 32 MIPS
	// Switchover will occur when PLL locks, according to config bits
	
	PORTA = 0;
	PORTB = 0;
	LATA = 0;
	LATB = 0;
	
	TRISA = 0b0000000000001111;
	TRISB = 0b0000001000001111;
	
	ANSELA = 0b00000000000000011;
	ANSELB = 0b00000000000001011;
	
	CNPUB = 0b0000000000000000;
	
	
	// Timer tick
	T2CONbits.TCKPS = 0b11; // 1:256
	PR2 = (FCY / 256 / 100); // 100 Hz
	T2CONbits.TON = 1;
	
	
	// Debug UART
	U1BRG = 68; // 115200 baud, Fcy/(4*baud)-1
	U1MODEbits.BRGH = 1;
	U1MODEbits.UARTEN = 1;
	U1STAbits.UTXEN = 1;

	
	// RPs
	OSCCONbits.IOLOCK = 0;
	RPINR7bits.IC1R = 41;
	RPINR7bits.IC2R = 41;
	RPOR4bits.RP42R = 0b010000; // OC1
	RPOR4bits.RP43R = 0b000001; // U1TX
	OSCCONbits.IOLOCK = 1;
	
	
	// TMR1 - unused
	// TMR2 - 100 Hz tick
	// TMR3 - PPM timer
	// TMR4 - IC (PWM) time source
	// TMR5 - ADC trigger
	
	
	// Interrupts
	IFS0bits.T3IF = 0;
	IFS0bits.T2IF = 0;
	IFS0bits.IC1IF = 0;
	IFS0bits.DMA0IF = 0;
	
	IPC2bits.T3IP = 5; // PPM timer
	IPC1bits.T2IP = 3; // 100 Hz tick
	IPC0bits.IC1IP = 5; // PWM input rising
	IPC1bits.DMA0IP = 4; // ADC measurements
	
	IEC0bits.T3IE = 1;
	IEC0bits.T2IE = 1;
	IEC0bits.IC1IE = 1;
	IEC0bits.DMA0IE = 1;
}
