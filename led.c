#include "led.h"

// Configures LED pins
void initLEDs() {
	P1DIR |= BIT4;
	P1OUT |= BIT4;
	P1OUT &= ~BIT4;

	P2DIR |= BIT0;
	P2OUT &= ~BIT0;
}

// Sets heading of LED array so that 3 are illuminated
inline void setHeading(directions dir) {
	// Given a 'center', calculate the left and right offsets
	const directions left = (directions) ((dir - 1) & 7);
	const directions right = (directions) ((dir + 1) & 7);

	// Set all of the LEDs indexed before the cluster to 0
	uint8_t i;

	// Grab the head of the array
	led *l = array;
	for (i = 0; i < left; i++) {
		l->active = 0;
		l++;
	}

	// Enable the cluster right where the pointer stopped
	l->active = 1; l++;
	l->active = 1; l++;
	l->active = 1; l++;

	// Disable all others past the cluster
	for (i = right + 1; i < 8; i++) {
		l->active = 0;
		l++;
	}
}

// Updates the duty cycle of the LED cluster
inline void updateDuty(void) {
	// Iterate through all lights in the table
	uint8_t i;

	// Grab the head of the LED array
	led *l = array;

	// Loop through the entries
	for (i = 0; i < 8; i++) {
		// If it is active, then parse its current state
		if (l->active) {
			// If enough time has passed in the PWM cycle, turn off the LED
			if (l->ticks > l->duty) {
				l->ticks = 0;
			} else {
				// Else leave it on and keep waiting
				l->ticks++;
			}
		}
		// Move to the next entry
		l++;
	}
}

/*	Given an angle measurement, can we 
	mathematically determine the LED that maps to it? 
	Indeed, we can with some math. Depending on the half
	the angle lies on, the formulas are different:
	
	if (theta between - pi / 4 and - pi) then
		led = - 8 * theta - 1
	else 
		led = - 8 * theta + 7

	Since theta is fixed between [-1, 1), we can use a Q11 to hold
	a value in [-8, 8) successfully and retain some precision. */

// Update functions for LED heading on the XY plane
extern _q15 theta, phi;
inline void updateOnTheta(void) {
	volatile directions dir;
	_q11 fpDir;

	// Downcast theta to Q11 and get 8 in Q11 format
	const volatile _q7 theta7 = _QtoQ7(theta);
	const volatile _q7 mapping = _Q7(- 8);

	// Angle comparisons with full precision
	const volatile _q15 mPi = _Q15(- 1);
	const volatile _q15 mPiFour = _Q15(- 0.25);

	// Check which formula to use and perform the mapping with the function
	if (mPi <= theta && theta <= mPiFour) {
		fpDir = _Q7mpy(theta7, mapping) - _Q7(1);
	} else {
		fpDir = _Q7mpy(theta7, mapping) + _Q7(7);
	} 

	// Extract the integer part of the output and set the heading to that
	dir = (directions)  (_Q7int(fpDir) & 7);
	setHeading(dir);
}
//
//inline void updateOnPhi(void) {
//	directions dir;
//	_q15 fpDir;
//	if (mPi <= phi && phi <= mPiFour) {
//		fpDir = _Q15mpyQX(phi, 7, mInvPiFour, 7) - _Q15(1);
//	} else {
//		fpDir = _Q15mpyQX(phi, 7, mInvPiFour, 7) + _Q15(7);
//	}
//
//	dir = (directions) _Q15int(fpDir);
//}

// Display the actual data on the array
inline void display(void) {
	int8_t i;
	uint8_t ledState = 0;
	led *l = array;
	for (i = 7; i >= 0; i--) {
		const uint8_t active = l->active;
		ledState |= (active << i);
	}
	send(ledState);
}
