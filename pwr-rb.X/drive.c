#include <xc.h>

#include <stdio.h>

#include "system.h"
#include "util.h"

#define PWM_DEADTIME 400 // ns
#define PWM_DTR (PWM_DEADTIME / (1000 / (FOSC / 1000000)))
#define PWM_PERIOD (3200) // 20 kHz
#define SET_RANGE (500)
#define SET_DEADBAND (20)
#define MAX_DUTY ((int16_t)(PWM_PERIOD * 0.95))

#define INPUT_SCALE_FACTOR (3)
#define MAX_VEL_CHANGE (6) // Maximum setpoint change per loop iteration e.g. 10 -> -full to full takes 4 seconds with 100 Hz loop

#define WATCHDOG_TIMEOUT (25) //Number of 1/LOOP_RATE units of no commands received before motors stopped
#define LOOP_RATE (100)

static uint16_t watchdog = WATCHDOG_TIMEOUT;
static int16_t drive_setpoint = 0; // Commanded output -500 to 500
static int16_t drive_value = 0; // Current output after acceleration limit

void drive_init(void)
{
	// Disable fault inputs
	__builtin_write_PWMSFR(&FCLCON1, 0b0000000000000011, &PWMKEY);
	__builtin_write_PWMSFR(&FCLCON2, 0b0000000000000011, &PWMKEY);
	
	PTPER = PWM_PERIOD;
	DTR1 = DTR2 = PWM_DTR;
	ALTDTR1 = ALTDTR2 = PWM_DTR;
	PDC1 = PDC2 = 0;
	
	// Enabled, complementary, no override
	__builtin_write_PWMSFR(&IOCON1, 0b1100000000000000, &PWMKEY);
	__builtin_write_PWMSFR(&IOCON2, 0b1100000000000000, &PWMKEY);
	
	PTCONbits.PTEN = 1;
}

void drive_set(int16_t set)
{
	watchdog = 0;
	
	// Cap values
	LIMIT(set, -SET_RANGE, SET_RANGE);
	
	if (set < SET_DEADBAND && set > -SET_DEADBAND) {
		set = 0;
	}
	
	drive_setpoint = set;
}

void drive_print_info(void)
{
	printf("Command: %d  Output: %d\r\n", drive_setpoint, drive_value);
}

static void drive_update(void)
{
	int16_t out = drive_setpoint;
	
	// Acceleration limit
	static int16_t out_prev = 0;
	int16_t diff = out - out_prev;
	if (diff > MAX_VEL_CHANGE) {
		diff = MAX_VEL_CHANGE;
	} else if (diff < -MAX_VEL_CHANGE) {
		diff = -MAX_VEL_CHANGE;
	}
	out = out_prev + diff;
	out_prev = out;
	
	drive_value = out;
	
	out = (out * 32 / 5);
	
	LIMIT(out, -MAX_DUTY, MAX_DUTY);
	
	if (out >= 0) {
		PDC2 = 0;
		PDC1 = out;
	} else {
		PDC1 = 0;
		PDC2 = -out;
	}
}

// Return 1 if watchdog has expired
int drive_watchdog_expired(void)
{
	if (watchdog >= WATCHDOG_TIMEOUT) {
		return 1;
	} else {
		return 0;
	}
}

void drive_loop(void)
{
	if (drive_watchdog_expired()) {
		// No commands received for a while. Stop motor
		drive_setpoint = 0;
	} else {
		watchdog++;
	}
	
	drive_update(); // Update PWM after applying acceleration limit

	IFS0bits.T2IF = 0;
}

int16_t drive_get(void)
{
	return drive_value;
}

int drive_is_full(void)
{
	return (drive_value == SET_RANGE || drive_value == -SET_RANGE) ? 1 : 0;
}

void drive_shutdown(void)
{
	// Float power stage
	// Enabled, complementary, override both sides to 0
	__builtin_write_PWMSFR(&IOCON1, 0b1100001100000000, &PWMKEY);
	__builtin_write_PWMSFR(&IOCON2, 0b1100001100000000, &PWMKEY);
}
