#include "math.h"

inline int32_t smul16(int16_t x, int16_t y) {
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

inline uint32_t mul16(uint16_t x, uint16_t y) {
	uint32_t ret = 0;
	uint32_t xShift = (uint32_t) x;
    while (y) {
        if (y & 1) {
            ret += xShift;
        }

        xShift <<= 1;
        y >>= 1;
    }
	return ret;
}

// Samples array
extern uint16_t samples[3];

// Processed data
uint32_t arr[3] = { 0 };

// EMA filter implementation in fixed point
inline void processData(void) {
	// Q0.16 alpha and 1 - alpha
	const uint16_t alpha = (uint16_t) (0.0625 * 65535);
	const uint16_t mAlpha = 65535 - alpha;

	uint8_t i;
	for (i = 0; i < 3; i++) {
		// Calculate Q0.10 x Q0.15 = Q0.25
		// Drop 9 bits to bring it to SQ0.16
		volatile uint32_t p1 = mul16(alpha, samples[2 - i]);

		// Calculate Q0.16 x Q0.16 = SQ0.32
		// Drop 16 bits to bring it down to Q0.16
		volatile uint32_t p2 = mul16(mAlpha, arr[i]);

		// Sum the two responses
		arr[i] = (p1 >> 9) + (p2 >> 16);
	}
}
