#ifndef LED_H_
#define LED_H_

#include <dependencies.h>
#include "spi.h"

typedef enum directions {
	north = 7,
	northeast = 6,
	east = 5,
	southeast = 4,
	south = 3,
	southwest = 2,
	west = 1,
	northwest = 0
} directions;

typedef enum orientations {
	faceN,
	faceNNE,
	faceE,
	faceSE,
	faceS,
	faceSW,
	faceW,
	faceNW
} orientations;

typedef struct led {
	uint16_t period;
	uint16_t duty;
	uint16_t ticks;
	uint8_t active;
} led;

led array[8];

void initLEDs(void);
void setHeading(directions dir);
void updateDuty(void);
void display(void);

#endif
