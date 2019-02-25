#ifndef SLEEP_H_
#define SLEEP_H_

#include <dependencies.h>

// Basic sleep function for less than 65536 cycles
inline void usleep(uint16_t cycles);

// Extended sleep function
void sleep(int32_t cycles);

#endif /* SLEEP_H_ */
