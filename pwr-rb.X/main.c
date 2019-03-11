#include <xc.h>

#include "system.h"
#include "pins.h"
#include "drive.h"
#include "ppm.h"
#include "measure.h"
#include "error.h"

#include <libpic30.h>

int main(void)
{
	system_init();
	drive_init();
	measure_init();
	__delay_ms(100);
	ppm_init();
	
	while (1) {
		int16_t drive_val = drive_get();
		
		if (drive_watchdog_expired() || drive_val == 0 || error_get()) {
			// No PWM, stopped or error
			LED_GRN = 1;
			LED_RED = 1;
		} else if (drive_val > 0) {
			// Forward
			LED_GRN = 1;
			LED_RED = 0;
		} else {
			// Reverse
			LED_GRN = 0;
			LED_RED = 1;
		}
		__delay_ms(250);
		ClrWdt();
		if (error_get()) {
			// Error
			LED_GRN = 0;
			LED_RED = 1;
		} else if (drive_watchdog_expired()) {
			// No PWM
			LED_GRN = 0;
			LED_RED = 0;
		} else if (drive_val == 0) {
			// Stopped
			LED_GRN = 1;
			LED_RED = 1;
		} else if (drive_is_full()) {
			if (drive_val > 0) {
				// Forward
				LED_GRN = 1;
				LED_RED = 0;
			} else {
				// Reverse
				LED_GRN = 0;
				LED_RED = 1;
			}
		} else {
			// Non full speed
			LED_GRN = 0;
			LED_RED = 0;
		}
		__delay_ms(250);
		ClrWdt();
		
		measure_slow_loop();
		
		measure_print_info();
		drive_print_info();
		error_print_info();
	}	
}

// 100 Hz tick
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
	drive_loop();
	measure_loop();
	
	// Setup PPM values:
	ppm_set(PPM_I_VOLTAGE, measure_voltage()); // Max ~1100
	ppm_set(PPM_I_CURRENT, measure_current() + 1024); // Max ~2100
	ppm_set(PPM_I_TEMPERATURE, (measure_temperature_degc() + 40) * 5); // Max 850
	ppm_set(PPM_I_ERROR, (uint16_t)error_get() * 100 + 100); // Max 700
	ppm_set(PPM_I_DRIVE, drive_get() + 600); // Max 1100
	ppm_output_train(); // Output PPM feedback data
	
	IFS0bits.T2IF = 0;
}
