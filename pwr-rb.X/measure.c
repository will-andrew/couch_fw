#include <xc.h>

#include <stdio.h>

#include "pins.h"
#include "measure.h"
#include "system.h"
#include "error.h"
#include "util.h"
#include "thermistor.h"

int16_t current_accumulator = 0;
uint16_t voltage_accumulator = 0;
uint16_t accumulator_samples = 0;

uint16_t voltage = 0; // 10 bit representation
int16_t current = 0; // 10 bit representation
int8_t temperature = 0; // degrees C

int32_t long_current_accumulator = 0;
uint16_t long_accumulator_samples = 0;

struct adc1_buf adc1_buf __attribute__((near, aligned(sizeof(struct adc1_buf))));

void measure_init(void)
{
	// ADC
	// Channel scan disabled, AVDD/AVSS, 4 channel convert, ALTS, SMPI = 1
	AD1CON2 = 0b0000001000000101;

	AD1CON3bits.ADRC = 0; // System clock
	AD1CON3bits.ADCS = 5; // TAD = TOSC * 6

	AD1CON4bits.ADDMAEN = 1; // DMA enabled
	AD1CON4bits.DMABL = ADC1_SAMPLES_BITS;

	// MUXA = TH1, VREF, VSNS, ISNS
	// MUXB = TH2, VREF, VSNS, ISNS
	AD1CHS123 = 0;
	AD1CHS0bits.CH0SA = AN_TH1;
	AD1CHS0bits.CH0SB = AN_TH2;
	
	// Enable, scatter/gather DMA, internal trigger, SIMSAM, right just, 10 bit
	AD1CON1 = 0b1000000010001110; // Triggered by timer 5, ASAM
	
	// DMA0 for ADC1
	DMA0REQ = 0b00001101; // ADC1
	DMA0STAH = 0;
	DMA0STAL = (uint16_t)&adc1_buf;
	DMA0PAD = (uint16_t)&ADC1BUF0;
	DMA0CNT = ADC1_SAMPLES * 4 - 1; //3;
	DMA0CON = 0b1000000000100000; // Enabled, word size, periph indirect, no pp
	
	// Clock source for ADC
	PR5 = (FCY / 1 / 90000);
	T5CONbits.TON = 1;
}

void measure_print_info(void)
{
	uint16_t volts = CODES_TO_VOLTS(voltage);
	int16_t amps = CODES_TO_AMPS(current);
	printf("Voltage: %d V  Current: %02d A  Temp: %d C\r\n", volts, amps, measure_temperature_degc());
}

// New ADC samples are ready
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{
	for (int i = 0; i < ADC1_SAMPLES; i++) {
		int16_t current = adc1_buf.isns[i] - adc1_buf.vref[i];
		int16_t current_limit = AMPS_TO_CODES(PEAK_CURRENT_TRIP_AMPS);
		if (current > current_limit || current < -current_limit) {
			error_set(ERROR_PEAK_CURRENT, current);
		}
		
		voltage_accumulator += adc1_buf.vsns[i] >> ADC1_SAMPLES_BITS;
		current_accumulator += current >> ADC1_SAMPLES_BITS;
	}
	
	accumulator_samples++;
	
	IFS0bits.DMA0IF = 0;
}

void measure_loop(void)
{
	static uint16_t old_accumulator_samples = 0;
	
	// Disable DMA interrupt while updating accumulators
	IEC0bits.DMA0IE = 0;
	uint16_t samples_accumulated = accumulator_samples - old_accumulator_samples;
	
	if (samples_accumulated == 0) { samples_accumulated = 56; }
	
	voltage = voltage_accumulator / samples_accumulated;
	current = current_accumulator / (int)samples_accumulated;
	
	// Reset accumulators
	voltage_accumulator = 0;
	current_accumulator = 0;
	IEC0bits.DMA0IE = 1;
	
	old_accumulator_samples = accumulator_samples;
	
	long_current_accumulator += current;
	long_accumulator_samples++;
	
	static int startup_timer = 0;
	if (startup_timer < 100) {
		// Allow grace time at startup for undervoltage checking while capacitor charges
		startup_timer++;
	} else {
		// Check for under voltage
		if (voltage < VOLTS_TO_CODES(LOW_VOLTAGE_TRIP_VOLTS)) {
			error_set(ERROR_LOW_VOLTAGE, voltage);
		}
	}
	
	// Check for over voltage
	if (voltage > VOLTS_TO_CODES(HIGH_VOLTAGE_TRIP_VOLTS)) {
		error_set(ERROR_HIGH_VOLTAGE, voltage);
	}
}

uint16_t measure_voltage(void)
{
	return voltage;
}

int16_t measure_current(void)
{
	return current;
}

int8_t measure_temperature_degc(void)
{
	uint16_t t1_sam = 0, t2_sam = 0;
	for (int i = 0; i < ADC1_SAMPLES; i++) {
		t1_sam += adc1_buf.th1[i];
		t2_sam += adc1_buf.th2[i];
	}
	t1_sam /= ADC1_SAMPLES;
	t2_sam /= ADC1_SAMPLES;
	
	int8_t t1 = thermistor_lookup(t1_sam);
	int8_t t2 = thermistor_lookup(t2_sam);
	
	temperature = MAX(t1, t2);
	
	return temperature;
}

void measure_slow_loop(void)
{
	// Get slow averaged current reading
	static uint16_t old_long_accumulator_samples = 0;
	uint16_t samples_accumulated = long_accumulator_samples - old_long_accumulator_samples;
	old_long_accumulator_samples = long_accumulator_samples;
	
	if (samples_accumulated == 0) { samples_accumulated = 100; }
	int16_t ave_current = __builtin_divsd(long_current_accumulator, samples_accumulated);
	
	__builtin_disi(6); // Disable interrupts while clearing the accumulator
	long_current_accumulator = 0;
	
	// Check average current limit
	int16_t current_limit = AMPS_TO_CODES(AVE_CURRENT_TRIP_AMPS);
	if (ave_current > current_limit || ave_current < -current_limit) {
		error_set(ERROR_AVE_CURRENT, ave_current);
	}
	
	// Check if temperature limit has been exceeded
	if (temperature > HIGH_TEMPERATURE_TRIP_DEGC) {
		error_set(ERROR_HIGH_TEMP, temperature);
	}
}
