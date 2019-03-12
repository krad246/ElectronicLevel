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
		*l = (led) { 80, 80, 0, 0 };
		l++;
	}
}

static uint8_t offset;

// Sets heading of LED array so that 3 are illuminated
inline void setHeading(directions dir) {
	// Given a 'center', calculate the left and right offsets
	const directions left = (directions) ((dir - offset + 8) & 7);
	const directions right = (directions) ((dir + offset + 8) & 7);

	int8_t i;
	led *l;

	// If the offset results in a full sweep of the circle, then illuminate all LEDs
	if (left == right) {
		// Start at the head of the array
		l = array;

		// Grab each LED and make it active
		for (i = 7; i >= 0; i--) {
			l->active = 1;
			l++;
		}
	} else {
		// Otherwise use modular indexing to access and set the correct LEDs

		// Set all of the LEDs covered by the fan to active
		for (i = right; i >= left; i = (i + 7) & 7) {
			l = array + i;
			l->active = 1;
		}

		// Disable all others
		for (i = left - 1; i >= right + 1; i = (i + 7) & 7) {
			l = array + i;
			l->active = 0;
		}
	}
}

static uint8_t duty;

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
				l->active = 0;
			} else {
				// Else leave it on and keep waiting
				l->ticks++;
			}
		}

		// Start a new cycle once we're overdue
		// Also change the duty for the next refresh period
		if (l->ticks > l->period) {
			l->ticks = 0;
			l->duty = duty;
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

// Possible phi directions
static const _q15 orientations[31] = {
		_Q15(-0.9375),
		_Q15(-0.875),
		_Q15(-0.8125),
		_Q15(-0.75),
		_Q15(-0.6875),
		_Q15(-0.625),
		_Q15(-0.5625),
		_Q15(-0.5),
		_Q15(-0.4375),
		_Q15(-0.375),
		_Q15(-0.3125),
		_Q15(-0.25),
		_Q15(-0.1875),
		_Q15(-0.125),
		_Q15(-0.0625),
		_Q15(0),
		_Q15(0.0625),
		_Q15(0.125),
		_Q15(0.1875),
		_Q15(0.25),
		_Q15(0.3125),
		_Q15(0.375),
		_Q15(0.4375),
		_Q15(0.5),
		_Q15(0.5625),
		_Q15(0.625),
		_Q15(0.6875),
		_Q15(0.75),
		_Q15(0.8125),
		_Q15(0.875),
		_Q15(0.9375)
};

static const uint8_t duties[30] = {
		75, 70, 65, 60,
		55, 50, 45, 40,
		35, 30, 25, 20,
		20, 20, 20, 20,
		20, 20, 20, 25,
		30, 35, 40, 45,
		50, 55, 60, 65,
		70, 75
};

static const uint8_t fans[30] = {
		1, 1, 1, 1,
		2, 2, 2, 2,
		3, 3, 3, 4,
		4, 4, 4, 4,
		4, 4, 4, 3,
		3, 3, 2, 2,
		2, 2, 1, 1,
		1, 1
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
	if (phi < orientations[0] || phi > orientations[30]) {
		duty = 80;
		return;
	}

	int8_t i;
	for (i = 29; i >= 0; i--) {
		const _q15 boundaryBot = orientations[29u - i];
		const _q15 boundaryTop = orientations[29u - (i + 1u)];

		if (phi > boundaryBot && phi < boundaryTop) {
			duty = duties[(uint8_t) i];
			offset = fans[(uint8_t) i];
		}
	}
}

// Display the actual data on the array
inline void display(void) {
	// Update the heading
	setHeading(dirTheta);

	// Bit vector representing the LEDs currently active
	volatile uint8_t ledState = 0;

	// Loop through all of the LEDs in the array from MSB to least
	int8_t i;
	led *l = array;
	for (i = 7; i >= 0; i--) {
		// Get the active status and place it at the appropriate bit location
		const uint8_t active = l->active;
		ledState |= (active << i);
		l++;
	}

	// Use SPI to send the values to the LED array and latch the data
	send(ledState);
}
