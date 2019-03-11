#include <xc.h>
#include <stdint.h>

#include "thermistor.h"
#include "system.h"


int8_t mtemps[4];

void read_thermistor_temps(void)
{
	mtemps[0] = thermistor_get(sum_samples(adc1_buf.th1) >> 4);
	mtemps[1] = thermistor_get(sum_samples(adc1_buf.th2) >> 4);
	mtemps[2] = thermistor_get(sum_samples(adc1_buf.th3) >> 4);
	mtemps[3] = thermistor_get(sum_samples(adc1_buf.th4) >> 4);
}
