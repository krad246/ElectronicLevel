/*
 * button.h
 *
 *  Created on: Feb 8, 2019
 *      Author: krad2
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <msp430.h>
#include <stdint.h>

// Number of unit delays to debounce for
#define THRESHOLD 50

void initButton(void);
extern inline void outputFunction(void);

#endif /* BUTTON_H_ */
