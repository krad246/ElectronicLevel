#include "led.h"

led array[8];

inline const char *toString(directions dir) {
    const char * const names[8] = {
       "northwest",
       "west",
       "southwest",
       "south",
       "southeast",
       "east",
       "northeast",
       "north"
    };

    return names[(uint8_t) dir];
}

// Configures LED pins
void initLEDs() {
	P1DIR |= BIT4;
	P1OUT |= BIT4;
	P1OUT &= ~BIT4;

	// Set up the latch pin
	P2DIR |= BIT0;
	P2OUT &= ~BIT0;

	// Initialize the LED structs
	int8_t i;
	led *l = array;
	for (i = 7; i >= 0; i--) {
		*l = (led) { 10, 10, 0, 0 };
		l++;
	}
}

volatile directions dirTheta;
static uint8_t offset;

// Sets heading of LED array so that 3 are illuminated
inline void setHeading(void) {
	// Given a 'center', calculate the left and right offset
	volatile const int8_t leftVal = dirTheta - offset;
	volatile const int8_t rightVal = dirTheta + offset;
	volatile const directions left = (directions) ((leftVal + 8) & 7);
	volatile const directions right = (directions) ((rightVal + 8) & 7);

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
		// Set all of the LEDs covered by the fan to active and all others to disabled
		l = array;
		for (i = 7; i >= 0; i--) {
			l->active = 0;
			l++;
		}

		for (i = rightVal; i >= leftVal; i--) {
			l = array + (i & 7);
			l->active = 1;
		}
	}
}

volatile uint8_t duty;

// Updates the duty cycle of the LED cluster
inline void updateTicks(void) {
	// Iterate through all lights in the table
	int8_t i;

	// Grab the head of the LED array
	led *l = array;

	// Loop through the entries
	for (i = 7; i >= 0; i--) {
        // Update the tick counter
        l->ticks++;

		// If it is active, then parse its current state
		if (l->active) {
			// If enough time has passed in the PWM cycle, turn off the LED
			if (l->ticks >= l->duty) {
				l->active = 0;
			}
		}

        // Start a new cycle once we're overdue
        if (l->ticks >= l->period) {
            l->ticks = 0;

            // Also change the duty for the next refresh period
            l->duty = duty;
        }

		// Move to the next entry
		l++;
	}
}

// List of directions on LED ring except for the point of wrapping
static const volatile directions mapping[7] = {
	southeast,
	east,
	northeast,
	north,
	northwest,
	west,
	southwest
};

// Adjacent entries correspond to a direction
static const volatile _q15 slices[8] = {
		_Q15(-0.4375),
		_Q15(-0.3125),
		_Q15(-0.1875),
		_Q15(-0.0625),
		_Q15(0.0625),
		_Q15(0.1875),
		_Q15(0.3125),
		_Q15(0.4375)
};

// Possible phi directions
static const _q15 orientations[15] = {
        _Q15(-0.125),
        _Q15(-0.09375),
        _Q15(-0.078125),
        _Q15(-0.0625),
        _Q15(-0.046875),
        _Q15(-0.03125),
        _Q15(-0.015625),
        _Q15(0),
        _Q15(0.015625),
        _Q15(0.03125),
        _Q15(0.046875),
        _Q15(0.0625),
        _Q15(0.078125),
        _Q15(0.09375),
        _Q15(0.125)
};

static const uint8_t duties[14] = {
        2, 3, 4, 5, 7, 8, 10, 10, 8, 7, 5, 4, 3, 2
};

static const uint8_t fans[14] = {
        4, 3, 3, 2, 1, 1, 1, 1, 2, 3, 3, 4
};

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
		volatile const _q15 boundaryTop = slices[7u - i];
		volatile const _q15 boundaryBot = slices[7u - (i + 1u)];

		// If theta lies within this range, set the heading and break
		if (theta > boundaryBot && theta < boundaryTop) {
			dirTheta = mapping[7u - (i + 1u)];
			break;
		}
	}
}

inline void updateOnPhi(void) {
	int8_t i;
	for (i = 13; i >= 0; i--) {
		volatile const _q15 boundaryTop = orientations[14u - i];
		volatile const _q15 boundaryBot = orientations[14u - (i + 1u)];

		if (phi > boundaryBot && phi < boundaryTop) {
			duty = duties[14u - (i + 1u)];
			offset = fans[14u - (i + 1u)];
		}
	}
}

// Display the actual data on the array
inline void display(void) {
	// Bit vector representing the LEDs currently active
	volatile uint8_t ledState = 0;

	// Loop through all of the LEDs in the array from MSB to least
	int8_t i;
	led *l = array;
	for (i = 7; i >= 0; i--) {
		// Get the active status and place it at the appropriate bit location
		const uint8_t active = l->active;
		ledState |= (active << (7u - i));
		l++;
	}

	// Use SPI to send the values to the LED array and latch the data
	send(ledState);
}
