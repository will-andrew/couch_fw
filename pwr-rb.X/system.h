/* 
 * File:   system.h
 * Author: will
 *
 * Created on 16 August 2017, 8:54 PM
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#include <stdint.h>

void system_init(void);

#define FOSC (64000000ULL)
#define FCY (FOSC / 2)

#define LOOP_RATE (100)

#endif	/* SYSTEM_H */

