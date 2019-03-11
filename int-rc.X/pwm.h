/* 
 * File:   pwm.h
 * Author: will
 *
 * Created on 22 August 2017, 12:35 AM
 */

#ifndef PWM_H
#define	PWM_H

void pwm_init(void);
void pwm_set_value(int index, uint16_t value);
void pwm_set_values(uint16_t *vals);

#endif	/* PWM_H */

