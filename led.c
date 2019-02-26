#include "led.h"

// Sets heading of LED array so that 3 are illuminated
void setHeading(directions dir) {
	// Given a 'center', calculate the left and right offsets
	const directions left = (directions) ((dir - 1) & 7);
	const directions right = (directions) ((dir + 1) & 7);

	// Set all of the LEDs indexed before the cluster to 0
	uint8_t i;
	for (i = 0; i < left; i++) array[i].active = 0;

	// Enable the cluster centered at 'dir'
	array[left].active = 1;
	array[dir].active = 1;
	array[right].active = 1;

	// Disable all others past the cluster
	for (i = right + 1; i < 8; i++) array[i].active = 0;
}

// Updates the duty cycle of the LED cluster
void updateDuty(void) {
	// Iterate through all lights in the table
	uint8_t i;
	for (i = 0; i < 8; i++) {
		// Grab each LED struct
		led l = array[i];

		// If it is active, then parse its current state
		if (l.active) {
			// If enough time has passed in the PWM cycle, turn off the LED
			if (l.ticks > l.duty) {
				l.ticks = 0;
			} else {
				// Else leave it on and keep waiting
				l.ticks++;
			}
		} 
	}
}

/*	Given an angle measurement, can we 
	mathematically determine the LED that maps to it? 
	Indeed, we can with some math. Depending on the half
	the angle lies on, the formulas are different:
	
	if (theta between - pi / 4 and - pi) then
		led = - 4 / pi * theta - 1
	else 
		led = - 4 / pi * theta + 7
	Thus we must calculate - 4 / pi in fixed point.
	To prevent overflows when multiplying, we have to cut precision.
	Q15 = Q7 * Q7 so that's what we have to keep our constants at.
	We need to calculate - pi, - pi / 4, and - 4 / pi. */

// Compute the necessary constants as outlined above
#define PI 3.14159265358979323846
const _q15 mPiFour = _Q7(- PI / 4.0);
const _q15 mInvPiFour = _Q7(- 4.0 / PI);
const _q15 mPi = _Q7(- PI);

#define FORMAT 7

extern _q15 theta, phi;

// Update functions for LED heading on the XY plane
inline void updateOnTheta(void) {
	directions dir;
	_q15 fpDir;

	const _q7 theta7 = _Q7(theta);

	// Check which formula to use and perform the mapping with the function
	if (mPi <= theta7 && theta7 <= mPiFour) {
		const int offset = 1;
		fpDir = _Q15mpyQX(theta7, FORMAT, mInvPiFour, FORMAT) - _Q15(offset);
	} else {
		const int offset = 7;
		fpDir = _Q15mpyQX(theta7, FORMAT, mInvPiFour, FORMAT) + _Q15(offset);
	} 

	// Extract the integer part of the output and set the heading to that
	dir = (directions) _Q15int(fpDir);
	setHeading(dir);
}

inline void updateOnPhi(void) {
	directions dir;
	_q15 fpDir;
	if (mPi <= phi && phi <= mPiFour) {
		fpDir = _Q15mpyQX(phi, 7, mInvPiFour, 7) - _Q15(1);
	} else {
		fpDir = _Q15mpyQX(phi, 7, mInvPiFour, 7) + _Q15(7);
	} 

	dir = (directions) _Q15int(fpDir);
}
