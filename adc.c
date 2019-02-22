/*
 * adc.c
 *
 *  Created on: Feb 21, 2019
 *      Author: krad2
 */

#include "adc.h"

void initADC(void) {
	// Switch to GPIO mode
	P1DIR &= ~(BIT1 | BIT2);
	P1SEL &= ~(BIT1 | BIT2);
	P1SEL2 &= ~(BIT1 | BIT2);

	// Shut the device off
	ADC10CTL0 &= ~ENC;

	// Start at pin 2 and read a sequence of channels
	ADC10CTL1 = INCH_2 | CONSEQ_1;

	// Read 3 channels, 2-1-0
	ADC10DTC1 = 3;
	ADC10AE0 = BIT0;

	// Turn on the device and enable it
	ADC10CTL0 = MSC | ADC10ON | ENC;
}

inline void UARTtoADC(void) {
	// Switch to GPIO mode
	P1DIR = 0x00;
	P1SEL = 0x00;
	P1SEL2 = 0x00;

	// Enable analog channel input
	ADC10AE0 = BIT1 | BIT2;
}

// Samples array for ADC
int16_t samples[3] = { 0 };
inline void readADC(void) {
	// Wait for unfinished transfers
	while (ADC10CTL1 & ADC10BUSY);

	// Provide the ADC the pointer to the array and start conversion
	ADC10SA = (unsigned int) samples;
	ADC10CTL0 |= ADC10SC;
}