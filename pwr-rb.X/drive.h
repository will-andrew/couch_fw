/* 
 * File:   drive.h
 * Author: will
 *
 * Created on 16 August 2017, 10:45 PM
 */

#ifndef DRIVE_H
#define	DRIVE_H

void drive_init(void);
void drive_loop(void);

void drive_set(int16_t set);
int drive_watchdog_expired(void);
int16_t drive_get(void);
int drive_is_full(void);
void drive_shutdown(void);
void drive_print_info(void);

#endif	/* DRIVE_H */

