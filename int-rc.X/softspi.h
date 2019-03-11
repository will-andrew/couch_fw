/* 
 * File:   softspi.h
 * Author: will
 *
 * Created on 22 August 2017, 2:15 PM
 */

#ifndef SOFTSPI_H
#define	SOFTSPI_H

#include <stdint.h>
#include <xc.h>

// Fake control registers
union SPICON1_soft {
	uint16_t v;
	SPI1CON1BITS bits;
};

union SPICON2_soft {
	uint16_t v;
	SPI1CON2BITS bits;
};

union SPISTAT_soft {
	uint16_t v;
	SPI1STATBITS bits;
};

extern union SPICON1_soft SPICON1_soft;
extern union SPICON2_soft SPICON2_soft;
extern union SPISTAT_soft SPISTAT_soft;
extern uint16_t SPIBUF_soft;
extern int IFS_soft;

void softspi_transact(void);
void softspi_transact_general(uint8_t *write, uint8_t write_len, uint8_t *write2, uint8_t write2_len, uint8_t *read, uint8_t read_len);

#endif	/* SOFTSPI_H */

