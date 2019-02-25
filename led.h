#ifndef LED_H_
#define LED_H_

#include <dependencies.h>

typedef enum directions {
	north = 7,
	northeast = 6,
	east = 5,
	southeast = 4,
	south = 3,
	southwest = 2,
	west = 1,
	northwest = 0
};

typedef struct led {
	uint16_t period;
	uint16_t duty;
	uint16_t ticks;
	uint8_t active;
};

led array[8];

void setHeading(directions dir);
void update(void);

#endif
