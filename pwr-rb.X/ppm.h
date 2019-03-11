/* 
 * File:   ppm.h
 * Author: will
 *
 * Created on 16 August 2017, 9:06 PM
 */

#ifndef PPM_H
#define	PPM_H

#define PPM_ITEMS (5)
#define SEQUENCE_DURATION (10000)

#define PPM_I_VOLTAGE (0)
#define PPM_I_CURRENT (1)
#define PPM_I_TEMPERATURE (2)
#define PPM_I_ERROR (3)
#define PPM_I_DRIVE (4)

void ppm_init(void);
void ppm_output_train(void);

extern uint16_t cmd_duration;

void ppm_set(uint8_t index, uint16_t val);

#endif	/* PPM_H */

