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

inline const char *toString(directions dir);

typedef struct led {
	uint16_t period;
	uint16_t duty;
	uint16_t ticks;
	uint8_t active;
} led;

void initLEDs(void);

inline void updateOnTheta(void);
inline void updateOnPhi(void);
inline void setHeading(void);
inline void updateTicks(void);
inline void display(void);

#endif
