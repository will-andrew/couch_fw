/* 
 * File:   encoder.h
 * Author: will
 *
 * Created on 20 August 2017, 9:42 AM
 */

#ifndef ENCODER_H
#define	ENCODER_H

void encoder_init(void);
void encoder_readall(int32_t *disps);

extern int encoder_failed;

#endif	/* ENCODER_H */

