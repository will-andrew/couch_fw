
#include "system.h"

#include <xc.h>

#include "time.h"
#include "message.h"

#define TICKS_TO_NS(t) ((t) * (1000000000 / FCY))
#define NS_TO_TICKS(n) ((n) / (1000000000 / FCY))

static uint32_t unix = 0;

void time_init(void)
{
	unix = 0;

	T4CONbits.T32 = 1;
	T4CONbits.TCKPS = 0;

	PR4 = LOW_WORD(FCY);
	PR5 = HIGH_WORD(FCY);

	T4CONbits.TON = 1;
}

void time_set(struct timestamp *timestamp)
{
	uint32_t ns = timestamp->tv_nsec;

	if (ns > 1000000000) {
		ns = 1000000000;
	}

	T4CONbits.TON = 0;

	TMR4 = LOW_WORD(NS_TO_TICKS(ns));
	TMR5 = HIGH_WORD(NS_TO_TICKS(ns));
	unix = timestamp->tv_sec;

	T4CONbits.TON = 1;
}

void time_get(struct timestamp *timestamp)
{
	uint32_t ulat = unix;

	uint16_t t5lat = TMR5;

	if (t5lat > TMR5) {
		ulat++;
	}

	uint16_t t4lat = TMR4;

	if (t4lat > TMR4) {
		t5lat++;

		if (t5lat == 0) {
			ulat++;
		}
	}

	timestamp->tv_sec = ulat;
	timestamp->tv_nsec = TICKS_TO_NS((uint32_t)t5lat << 16 | t4lat);
}

uint32_t time_get_seconds(void)
{
	return unix;
}

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void)
{
	unix++;
	IFS1bits.T5IF = 0;
}
