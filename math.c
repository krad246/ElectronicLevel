#include "math.h"

inline int32_t mul16(int16_t x, int16_t y) {
    uint8_t negative = 0;
	if (x < 0) {
		negative = !negative;
		x = -x;
	}

	if (y < 0) {
		negative = !negative;
		y = -y;
	}

	int32_t ret = 0;
	while (y > 0) {
		if (y & 0x01) {
			ret += x;
		}
		x <<= 1;
		y >>= 1;

	}

	if (negative) {
		ret = -ret;
	}

	return ret;
}

// Samples array
extern int16_t samples[3];

// Processed data
int16_t arr[3] = { 0 };

// EMA filter implementation in fixed point Q0.15 x Q0.9
inline void processData(void) {
	// Signed Q0.15 alpha and 1 - alpha
	const int16_t alpha = (int16_t) (0.0625 * 32767);
	const int16_t mAlpha = 32767 - alpha;

	uint8_t i;
	for (i = 0; i < 3; i++) {
		// Calculate SQ0.10 x SQ0.15 = SQ0.25
		// Drop 10 bits to bring it down to SQ0.15
		int32_t p1 = mul16(alpha, samples[2 - i]);

		// Multiply the SQ0.15 x SQ0.15 and drop 15 bits to bring it to SQ0.15
		int32_t p2 = mul16(mAlpha, arr[i]);

		// Both products are SQ0.15 so they can be added
		arr[i] = (p2 >> 15) + (p1 >> 10);
	}

	arr[0] = mul16(alpha, samples[2]) + mul16(mAlpha, arr[0]);
	arr[1] = mul16(alpha, samples[1]) + mul16(mAlpha, arr[1]);
	arr[2] = mul16(alpha, samples[0]) + mul16(mAlpha, arr[2]);
}
