/* 
 * File:   pins.h
 * Author: will
 *
 * Created on 15 August 2017, 11:37 PM
 */

#ifndef PINS_H
#define	PINS_H

#define AN_VREF (0)
#define AN_ISNS (1)
#define AN_VSNS (2)
#define AN_TH1 (3)
#define AN_TH2 (5)

#define LED_RED (LATBbits.LATB7)
#define LED_GRN (LATBbits.LATB8)

//#define JP (PORTBbits.RB11)

#define RC_IN (PORTBbits.RB9)

#endif	/* PINS_H */

