#include <xc.h>
#include <stdio.h>

#include "system.h"
#include "pins.h"
#include "util.h"

struct adc1_buf __attribute__((near, aligned(sizeof(struct adc1_buf)))) adc1_buf;

int __C30_UART = 2; // STDIO goes to UART 2

void interrupts_init(void);

void system_init(void)
{
// Oscillator
	CLKDIVbits.PLLPRE = 6;					// Divide by 8, 2 MHz
	PLLFBD = 138;							// Multiply by 140, 280 MHz
	CLKDIVbits.PLLPOST = 0;					// Divide by 2, 70 MIPS
	// Switchover will occur when PLL locks, according to config bits

	// IO
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	PORTE = 0;
	PORTF = 0;
	PORTG = 0;

	TRISA = 0b0001100000010001;
	CNPUA = 0b0000000000000000;
	TRISB = 0b1011110111101011;
	CNPUB = 0b1011110111101000;
	TRISC = 0b1001001111000000;
	CNPUC = 0b0000001111000000;
	TRISD = 0b0000000001100000;
	CNPUD = 0b0000000001100000;
	TRISE = 0b1110000000000000;
	CNPUE = 0b0000000000000000;
	TRISF = 0b0000000000000001;
	CNPUF = 0b0000000000000001;
	TRISG = 0b0000000000000000;
	CNPUG = 0b0000000000000000;
	
	CNPDBbits.CNPDB3 = 1; // Pull down MISO
	
	ANSELA = 0b0001100000010000;
	ANSELB = 0b0000000000000000;
	ANSELC = 0b0000000000000000;
	ANSELD = 0b0000000000000000;
	ANSELE = 0b1110000000000000;
	ANSELF = 0b0000000000000000;
	ANSELG = 0b0000000000000000;
	
	LED_STAT1 = 1;
	
	// DMA1 for ADC1
	DMA1REQ = 0b00001101; // ADC1
	DMA1STAH = 0;
	DMA1STAL = (uint16_t)&adc1_buf;
	DMA1PAD = (uint16_t)&ADC1BUF0;
	DMA1CNT = ADC1_SAMPLES * 32 - 1;
	DMA1CON = 0b1000000000100000; // Enabled, word size, periph indirect, no pp
	
	// ADC1
	AD1CON3bits.ADRC = 0; 
	AD1CON3bits.SAMC = 9;
	AD1CON3bits.ADCS = 4;

	AD1CON4bits.ADDMAEN = 1; // DMA enabled
	AD1CON4bits.DMABL = ADC1_SAMPLES_BITS;

	uint32_t css = BIT(AN_TH1) | BIT(AN_TH2) | BIT(AN_TH3) | BIT(AN_TH4) | BIT(AN_VBAT) | BIT(AN_ESTOP);
	AD1CSSH = css >> 16;
	AD1CSSL = css & 0xFFFF;
	
	AD1CON2bits.CSCNA = 1; // Use channel scan
	AD1CON2bits.SMPI = 5; // 6 channels enabled for channel scan

	// Enable, scatter/gather DMA, ASAM, int trigger, right just, 10 bit
	AD1CON1 = 0b1000000011100110;
	
	// CN
	CNENBbits.CNIEB11 = 1; // QEI1
	CNENDbits.CNIED6 = 1; // QEI2
	CNENCbits.CNIEC7 = 1; // QEI3
	CNENBbits.CNIEB5 = 1; // QEI4
	
	// UARTs
	// UART2 - debug
	U2MODE = 0;
	U2MODEbits.UARTEN = 1;
	U2MODEbits.BRGH = 1;
	U2STA = 0;
	U2STAbits.UTXEN = 1;
	U2BRG = 151; // 115200 baud
	
	// RP (UART, QEI, IC, OC)
	RPINR19bits.U2RXR = 119;
	RPOR10bits.RP118R = 0b000011; // U2TX
	
	RPINR14bits.QEA1R = 57; // ENC2A
	RPINR14bits.QEB1R = 96; // ENC2B
	//RPINR15bits.INDX1R = 70;
	RPINR16bits.QEA2R = 38; // ENC4A
	RPINR16bits.QEB2R = 54; // ENC4B
	//RPINR17bits.INDX2R = 37;
	
	RPINR7bits.IC1R = 44; // ENC1A
	RPINR7bits.IC2R = 45; // ENC1B
	RPINR8bits.IC3R = 56; // ENC3A
	RPINR8bits.IC4R = 69; // ENC3B
	
	RPINR9bits.IC5R = 47; // M1FB, pin 3
	RPINR9bits.IC6R = 42; // M2FB, pin 60
	RPINR10bits.IC7R = 40; // M3FB, pin 48
	RPINR10bits.IC8R = 39; // M4FB, pin 46
	
	// Timers
	// TMR1 used by TCP/IP stack
	
	// TMR2 OC timebase
	T2CONbits.TCKPS = 0b10; // 1:64
	T2CONbits.TON = 1;
	
	// TMR3 (stream tick)
	T3CONbits.TCKPS = 0b11;			// 1:256
	T3CONbits.TON = 0;				// Don't enable until we start streaming
	
	// TMR4+5 used in time.c
	
	// TMR6+7 used in encoder.c/encoder-ic.s
	
	// Interrupt setup
	interrupts_init();
}

void interrupts_init(void)
{
	IFS0bits.T3IF = 0;
	IFS1bits.T5IF = 0;
	IFS0bits.IC1IF = 0;
	IFS0bits.IC2IF = 0;
	IFS2bits.IC3IF = 0;
	IFS2bits.IC3IF = 0;
	IFS2bits.IC4IF = 0;
	IFS1bits.CNIF = 0;
	IFS2bits.IC5IF = 0;
	IFS2bits.IC6IF = 0;
	IFS1bits.IC7IF = 0;
	IFS1bits.IC8IF = 0;
	IFS0bits.OC1IF = 0;
	IFS0bits.OC2IF = 0;
	IFS1bits.OC3IF = 0;
	IFS1bits.OC4IF = 0;
	IFS2bits.OC5IF = 0;
	IFS2bits.T6IF = 0;
	IFS3bits.T7IF = 0;

	/* Priority
	 * 7: PWM (OC)
	 * 6: T6, T7 (SW encoder watchdog)
	 * 5: T5 (update unix)
	 * 4: Encoder index, software decoding
	 * 3: PPM (IC so latency unimportant)
	 * 2: T3 (stream tick), T1 tick for TCP stack
	 * 1:
	 */

	IPC2bits.T3IP = 2; // Stream tick
	IPC7bits.T5IP = 5; // Timestamp
	// Encoder
	IPC0bits.IC1IP = 4;
	IPC1bits.IC2IP = 4;
	IPC9bits.IC3IP = 4;
	IPC9bits.IC4IP = 4;
	IPC4bits.CNIP = 4;
	IPC11bits.T6IP = 6;
	IPC12bits.T7IP = 6;
	// PPM timers
	IPC9bits.IC5IP = 3;
	IPC10bits.IC6IP = 3;
	IPC5bits.IC7IP = 3;
	IPC5bits.IC8IP = 3;
	// OC for PWM
	IPC0bits.OC1IP = 6;
	IPC1bits.OC2IP = 6;
	IPC6bits.OC3IP = 6;
	IPC6bits.OC4IP = 6;
	IPC10bits.OC5IP = 6;

	IEC0bits.T3IE = 1;
	IEC1bits.T5IE = 1;
	IEC0bits.IC1IE = 1;
	IEC0bits.IC2IE = 1;
	IEC2bits.IC3IE = 1;
	IEC2bits.IC4IE = 1;
	//IEC1bits.CNIE = 1;
	IEC2bits.IC5IE = 1;
	IEC2bits.IC6IE = 1;
	IEC1bits.IC7IE = 1;
	IEC1bits.IC8IE = 1;
	IEC0bits.OC1IE = 1;
	IEC0bits.OC2IE = 1;
	IEC1bits.OC3IE = 1;
	IEC1bits.OC4IE = 1;
	IEC2bits.OC5IE = 1;
	IEC2bits.T6IE = 1;
	IEC3bits.T7IE = 1;
}

uint16_t sum_samples(uint16_t *buf)
{
	uint16_t sum = 0;
	for (int i = 0; i < ADC1_SAMPLES; i++) {
		sum += buf[i];
	}
	return sum;
}

void (*getErrLoc(void))(void);  // Get Address Error Loc

void __attribute__((interrupt, no_auto_psv)) _AddressError(void)
{
		void (*pc)(void);
		pc = getErrLoc();	// get the location of error function
		
		printf("Address error at 0x%x\r\n", (uint16_t)pc);
		
        INTCON1bits.ADDRERR = 0;	//Clear the trap flag
		
		while(1);
}
