#include "math.h"

// Samples array
extern _q15 samples[3];

// Processed data
_q15 arr[3] = { 0 };

extern void readADC();

// EMA filter implementation in fixed point
inline void processData(void) {
	// Collect the data for processing
	readADC();

	// Q0.15 alpha and 1 - alpha
	const volatile _q15 alpha = _Q15(0.0625);
	const volatile _q15 mAlpha = _Q15(1 - 0.0625);

	uint8_t i;
	for (i = 0; i < 3; i++) {
		// Calculate alpha * s[i]
		volatile _q15 p1 = _Q15mpy(alpha, samples[2 - i]);

		// Calculate (1 - alpha) * y[n - 1]
		volatile _q15 p2 = _Q15mpy(mAlpha, arr[i]);

		// Sum the two responses
		arr[i] = p1 + p2;
	}
}

// Calculate the angle of the device in fixed point
extern _q15 x0, y0, z0;
_q11 theta, phi;
inline void getAngles(void) {
	// Calculate vector displacement
	const volatile _q15 dx = arr[0] - x0;
	const volatile _q15 dy = arr[1] - y0;
	const volatile _q15 dz = arr[2] - z0;

	// Calculate xy-plane radius
	const volatile _q15 r = _Q15mag(dx, dy);

	// Calculate 3d radius
	const volatile _q15 rho = _Q15mag(r, dz);

	// Compute theta and phi using the lengths, normalize it to [-1, 1)
	theta = _Q15atan2PU(dy, dx);
	phi = _Q15atan2PU(dz, rho);
}
