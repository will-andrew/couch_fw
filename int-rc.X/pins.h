/* 
 * File:   pins.h
 * Author: will
 *
 * Created on 11 August 2017, 8:21 PM
 */

#ifndef PINS_H
#define	PINS_H

#include "softspi.h"

#define PASTER(reg, el) reg##bits.##reg##el
#define ENCODER_REG(reg, el) (PASTER(reg, el))
#define QEA1 A1

#define GPIO_0 LATGbits.LATG6
#define GPIO_1 LATGbits.LATG7
#define GPIO_2 LATGbits.LATG8
#define GPIO_3 LATGbits.LATG9

#define AN_TH1 (24) // RA4/33
#define AN_TH2 (13) // RE13/28
#define AN_TH3 (14) // RE14/29
#define AN_TH4 (15) // RE15/30
#define AN_VBAT (10) // RA12/pin 11
#define AN_ESTOP (9) // RA11/pin 12

#define ESTOP (PORTAbits.RA11)
#define ESTOP_ENGAGE (LATDbits.LATD8)

#define LED_STAT1 (LATCbits.LATC1)
#define LED_STAT2 (LATCbits.LATC2)

#define LORA_TX (LATBbits.LATB4)
#define LORA_RX (LATEbits.LATE12)
#define LORA_RXDONE (PORTCbits.RC15)

#define QEI1 (PORTBbits.RB11)
#define QEI2 (PORTDbits.RD6)
#define QEI3 (PORTCbits.RC3)
#define QEI4 (PORTBbits.RB5)

#define M1RC (LATBbits.LATB14)
#define M2RC (LATFbits.LATF1)
#define M3RC (LATCbits.LATC13)
#define M4RC (LATCbits.LATC10)

#define SPI_SCK (LATBbits.LATB2)
#define SPI_MOSI (LATCbits.LATC0)
#define SPI_MISO (PORTBbits.RB3)
#define SPI_CS_ETH_TRIS (TRISAbits.TRISA1)
#define SPI_CS_ETH (LATAbits.LATA1)
#define SPI_CS_LORA (LATCbits.LATC11)

#define ENC_CS_TRIS			(SPI_CS_ETH_TRIS)
#define ENC_CS_IO			(SPI_CS_ETH)
#define ENC_SPI_IF			(IFS_soft)
#define ENC_SSPBUF			(SPIBUF_soft)
#define ENC_SPISTAT			(SPISTAT_soft.v)
#define ENC_SPISTATbits     (SPISTAT_soft.bits)
#define ENC_SPICON1			(SPICON1_soft.v)
#define ENC_SPICON1bits     (SPICON1_soft.bits)
#define ENC_SPICON2			(SPICON2_soft.v)
#define ENC_SPICON2bits     (SPICON2_soft.bits)

#endif	/* PINS_H */

