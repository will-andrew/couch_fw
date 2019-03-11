#include <xc.h>

#include <stdio.h>

#include "error.h"
#include "drive.h"
#include "measure.h"

static uint8_t error_code = ERROR_NONE;
static int16_t error_value = 0;

void error_set(uint8_t error, int16_t value)
{
	error_code = error;
	error_value = value;
	if (error_code > 0) {
		drive_shutdown();
	}
}

uint8_t error_get(void)
{
	return error_code;
}

void error_print_info(void)
{
	switch (error_code) {
		case ERROR_NONE:
			printf("No error\r\n");
			break;
		case ERROR_PEAK_CURRENT:
			printf("Peak current trip (%d A)\r\n", CODES_TO_AMPS(error_value));
			break;
		case ERROR_AVE_CURRENT:
			printf("Average current trip (%d A)\r\n", CODES_TO_AMPS(error_value));
			break;
		case ERROR_HIGH_VOLTAGE:
			printf("High voltage trip (%d V)\r\n", CODES_TO_VOLTS(error_value));
			break;
		case ERROR_LOW_VOLTAGE:
			printf("Low voltage trip (%d V)\r\n", CODES_TO_VOLTS(error_value));
			break;
		case ERROR_HIGH_TEMP:
			printf("High temperature trip (%d C)\r\n", error_value);
			break;
	}
}
