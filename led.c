#include "led.h"

void setHeading(directions dir) {
	const directions left = (dir - 1) & 7;
	const directions right = (dir + 1) & 7;

	uint8_t i;
	for (i = 0; i < left; i++) {
		led l = array[i];
		l.active = 0;
	}

	array[left].active = 1;
	array[dir].active = 1;
	array[right] = 1;

	for (i = right + 1; i < 8; i++) {
		led l = array[i];
		l.active = 0;
	}
}

void update(void) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		led l = array[i];
		if (l.active) {
			if (l.ticks > l.duty) {
				// outputFunction()
				l.ticks = 0;
			}
		} else l.ticks++;
	}
}
