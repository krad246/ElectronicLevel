/*
 * sleep.c
 *
 *  Created on: Feb 8, 2019
 *      Author: krad2
 */

#include "sleep.h"

// Basic sleep function for less than 65536 cycles
inline void usleep(uint16_t cycles) {
	// Stop and clear timer A
	TA0CTL = MC_0 | TACLR;

	// Set the number of cycles to count for
	TA0CCR0 = cycles;

	// Enable the highest priority interrupt
	TA0CCTL0 = CCIE;

	// Start the timer on the SMCLK counting up
	TA0CTL = TASSEL_2 | MC_1;

	// Enter LPM0 with interrupts enabled
	__low_power_mode_0();
}

// Extended sleep function
void sleep(int32_t cycles) {
	// Call usleep() ceil[cycles / 65535] times
	while (cycles > 0) {
		usleep(65535);
		cycles -= 65535;
	}
}

// Timer ISR to reawaken the CPU
#pragma vector = TIMER0_A0_VECTOR
interrupt void sleepCallback(void) {
	// Stop and clear the timer
	TA0CTL = MC_0 | TACLR;

	// Disable and clear interrupts
	TA0CCTL0 &= ~(CCIE | CCIFG);

	// Disable LPM0
	__low_power_mode_off_on_exit();
}
