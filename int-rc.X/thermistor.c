/*
 * Couch integrated motor controller firmware
 *
 * Hardware: couch-int-ra/9
 *
 * W. A. 1/14
 *
 * revA 23/1/14
 *
 * Notes:
 */

#include <xc.h>

#include <stdint.h>

#include "thermistor.h"

#define MIN_TEMP 0
#define MAX_TEMP 100
#define MIN_ADC 16
#define MAX_ADC 196

// Thermistor lookup table for 0-100 degrees, B=3950, 4.7k thermistor,
// Thermistor is lower in 4.7k divider, ADC is 8 bit.
// Data from http://www.thermistor.com/calculators?r=rtcr

const unsigned char thermLookup[][2] = {
{0,196},
{1,194},
{2,191},
{3,189},
{4,186},
{5,184},
{6,181},
{7,178},
{8,176},
{9,173},
{10,170},
{11,168},
{12,165},
{13,162},
{14,159},
{15,156},
{16,154},
{17,151},
{18,148},
{19,145},
{20,142},
{21,139},
{22,136},
{23,134},
{24,131},
{25,128},
{26,125},
{27,122},
{28,120},
{29,117},
{30,114},
{31,112},
{32,109},
{33,106},
{34,104},
{35,101},
{36,99},
{37,96},
{38,94},
{39,91},
{40,89},
{41,87},
{42,84},
{43,82},
{44,80},
{45,78},
{46,76},
{47,74},
{48,72},
{49,70},
{50,68},
{51,66},
{52,64},
{53,62},
{54,61},
{55,59},
{56,57},
{57,56},
{58,54},
{59,53},
{60,51},
{61,50},
{62,48},
{63,47},
{64,45},
{65,44},
{66,43},
{67,42},
{68,40},
{69,39},
{70,38},
{71,37},
{72,36},
{73,35},
{74,34},
{75,33},
{76,32},
{77,31},
{78,30},
{80,29},
{81,28},
{82,27},
{83,26},
{85,25},
{86,24},
{88,23},
{89,22},
{91,21},
{93,20},
{95,19},
{97,18},
{98,17},
{100,16}};

uint8_t thermistor_get(uint8_t adc)
{
	unsigned int i = 0;

	if(adc < MIN_ADC) {
		return MAX_TEMP;
	}

	while (adc < thermLookup[i][1]) {
		i++;
	}

	return thermLookup[i][0];
}
