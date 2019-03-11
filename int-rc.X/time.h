/* 
 * File:   time.h
 * Author: will
 *
 * Created on 12 April 2015, 6:56 AM
 */

#ifndef TIME_H
#define	TIME_H

#include <stdint.h>
#include "message.h"

void time_init(void); // Init time stuff, incl. peripherals
void time_set(struct timestamp *timestamp);
void time_get(struct timestamp *timestamp);
uint32_t time_get_seconds(void);

#endif	/* TIME_H */

