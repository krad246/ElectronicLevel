#include "led.h"

led array[8];

// Configures LED pins
void initLEDs() {
	P1DIR |= BIT4;
	P1OUT |= BIT4;
	P1OUT &= ~BIT4;

	P2DIR |= BIT0;
	P2OUT &= ~BIT0;

	int8_t i;
	led *l = array;
	for (i = 7; i >= 0; i--) {
		*l = (led) { 0, 0, 0, 0 };
		l++;
	}
}

// Sets heading of LED array so that 3 are illuminated
inline void setHeading(directions dir) {
	// Given a 'center', calculate the left and right offsets
	const directions left = (directions) ((dir - 1) & 7);
	const directions right = (directions) ((dir + 1) & 7);

	// Set all of the LEDs indexed before the cluster to 0
	int8_t i;

	// Grab the head of the array
	led *l = array;
	for (i = left - 1; i >= 0; i--) {
		l->active = 0;
		l++;
	}

	// Enable the cluster right where the pointer stopped
	l->active = 1; l++;
	l->active = 1; l++;
	l->active = 1; l++;

	// Disable all others past the cluster
	for (i = 7; i >= right + 1; i--) {
		l->active = 0;
		l++;
	}
}

// Updates the duty cycle of the LED cluster
inline void updateTicks(void) {
	// Iterate through all lights in the table
	int8_t i;

	// Grab the head of the LED array
	led *l = array;

	// Loop through the entries
	for (i = 7; i >= 0; i--) {
		// If it is active, then parse its current state
		if (l->active) {
			// If enough time has passed in the PWM cycle, turn off the LED
			if (l->ticks > l->duty) {
				l->ticks = 0;
				l->active = 0;
			} else {
				// Else leave it on and keep waiting
				l->ticks++;
			}
		}
		// Move to the next entry
		l++;
	}
}

// List of directions on LED ring except for the point of wrapping
static const directions mapping[7] = {
	southeast,
	east,
	northeast,
	north,
	northwest,
	west,
	southwest
};

// Adjacent entries correspond to a direction
static const _q15 slices[8] = {
		_Q15(-0.875),
		_Q15(-0.625),
		_Q15(-0.375),
		_Q15(-0.125),
		_Q15(0.125),
		_Q15(0.375),
		_Q15(0.625),
		_Q15(0.875)
};

static directions dirTheta;

// Function to update the heading of the LED ring
extern _q15 theta, phi;
inline void updateOnTheta(void) {
	// If the angle theta is less than -165 degrees or greater than 165 degrees it's south
	// Set the direction and leave; we're done
	if (theta < slices[0] || theta > slices[7]) {
		dirTheta = south;
		return;
	}

	// Otherwise compare the angle to every interval and check where it lies
	int8_t i;
	for (i = 6; i >= 0; i--) {
		// Moving counterclockwise, the 'bottom' and 'top' of the range
		const _q15 boundaryBot = slices[6u - i];
		const _q15 boundaryTop = slices[6u - (i + 1u)];

		// If theta lies within this range, set the heading and break
		if (theta > boundaryBot && theta < boundaryTop) {
			dirTheta = mapping[6u - i];
			break;
		}
	}
}

inline void updateOnPhi(void) {
	// If the angle phi is less than -165 degrees or greater than 165 degrees it's south
	// Set the direction and leave; we're done
	if (phi < slices[0] || phi > slices[7]) {
		return;
	}
}

// Display the actual data on the array
inline void display(void) {
	// Update the heading
	setHeading(dirTheta);

	// Bit vector representing the LEDs currently active
	uint8_t ledState = 0;

	// Loop through all of the LEDs in the array from MSB to least
	int8_t i;
	led *l = array;
	for (i = 7; i >= 0; i--) {
		// Get the active status and place it at the appropriate bit location
		const uint8_t active = l->active;
		ledState |= (active << i);
	}

	// Use SPI to send the values to the LED array and latch the data
	send(ledState);
}