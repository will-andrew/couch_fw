#include <xc.h>
#include <stdint.h>
#include <stdlib.h>

#include "system.h"

#include <libpic30.h>

#include "pins.h"
#include "softspi.h"

#define SPI_DELAY_US (0)

// Fake control registers
union SPICON1_soft SPICON1_soft;
union SPICON2_soft SPICON2_soft;
union SPISTAT_soft SPISTAT_soft;
uint16_t SPIBUF_soft = 0;
int IFS_soft = 0;

static void transact_simultaneous(uint8_t *buf, uint8_t length)
{
	for (uint8_t j = 0; j < length; j++) {
		uint8_t send_byte = buf[j];
		uint8_t recv_byte = 0;
		
		for (uint8_t i = 0; i < 8; i++) {
			SPI_SCK = 0;
			
			asm("btss %0,#7" : : "g"(send_byte));
			asm("bclr LATC,#0"); // MOSI
			asm("btsc %0,#7" : : "g"(send_byte));
			asm("bset LATC,#0");

			send_byte <<= 1;
			
			SPI_SCK = 1;
			
			recv_byte <<= 1;
			
			asm("btsc PORTB,#3"); // MISO
			asm("bset %0,#0" : "+g"(recv_byte));
		}
		
		buf[j] = recv_byte;
	}
	SPI_SCK = 0;
}

void softspi_transact(void)
{
	if (SPICON1_soft.bits.MODE16) {
		asm("swap %0" : "+r"(SPIBUF_soft));
		transact_simultaneous((uint8_t *)&SPIBUF_soft, 2);
		asm("swap %0" : "+r"(SPIBUF_soft));
	} else {
		transact_simultaneous((uint8_t *)&SPIBUF_soft, 1);
	}
}

void softspi_transact_general(uint8_t *write, uint8_t write_len, uint8_t *write2, uint8_t write2_len, uint8_t *read, uint8_t read_len)
{
	for (uint8_t j = 0; j < write_len; j++) {
		uint8_t byte = write[j];
		for (uint8_t i = 0; i < 8; i++) {
			SPI_SCK = 0;
			SPI_MOSI = (byte & 128) ? 1 : 0;
			byte <<= 1;
			__delay_us(SPI_DELAY_US);
			SPI_SCK = 1;
			__delay_us(SPI_DELAY_US);		
		}
	}
	
	for (uint8_t j = 0; j < write2_len; j++) {
		uint8_t byte = write2[j];
		for (uint8_t i = 0; i < 8; i++) {
			SPI_SCK = 0;
			SPI_MOSI = (byte & 128) ? 1 : 0;
			byte <<= 1;
			__delay_us(SPI_DELAY_US);
			SPI_SCK = 1;
			__delay_us(SPI_DELAY_US);		
		}
	}
	
	for (uint8_t j = 0; j < read_len; j++) {
		uint8_t byte = 0;
		for (uint8_t i = 0; i < 8; i++) {
			SPI_SCK = 0;
			SPI_MOSI = 0;
			__delay_us(SPI_DELAY_US);
			SPI_SCK = 1;
			byte <<= 1;
			if (SPI_MISO) {
				byte |= 1;
			}
			__delay_us(SPI_DELAY_US);		
		}
		read[j] = byte;
	}
	
	SPI_SCK = 0;
	__delay_us(SPI_DELAY_US);
}