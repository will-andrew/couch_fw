/* 
 * File:   ppm.h
 * Author: William Andrew
 *
 * Created on 24 August 2017, 7:54 PM
 */

#ifndef PPM_H
#define	PPM_H

#include <stdint.h>

#define N_ITEMS (5) // i.e. data items in a ppm sequence coming in
#define N_CHANNELS (4)

#define PPM_I_VOLTAGE (0)
#define PPM_I_CURRENT (1)
#define PPM_I_TEMPERATURE (2)
#define PPM_I_ERROR (3)
#define PPM_I_DRIVE (4)

void ppm_init(void);

void ppm_restart_interrupts(void);
void ppm_pause_interrupts(void);

extern uint16_t ppm_vals_hld[N_CHANNELS][N_ITEMS];

#endif	/* PPM_H */

