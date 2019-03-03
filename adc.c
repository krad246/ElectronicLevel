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
	ADC10CTL0 = MSC | ADC10ON | ENC | ADC10IE;
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
_q15 samples[3] = { 0 };
inline void readADC(void) {
	// Provide the ADC the pointer to the array and start conversion
	ADC10SA = (unsigned int) samples;
	ADC10CTL0 |= ADC10SC;
}

// Data processing task
extern void filter(void);

// Computing angles task
extern void getOrientation(void);

// After a read is performed successfully, do the math
#pragma vector = ADC10_VECTOR
interrupt void onRead(void) {
	filter();
	getOrientation();
}
