/*
 * adc.h
 *
 *  Created on: Feb 21, 2019
 *      Author: krad2
 */

#ifndef ADC_H_
#define ADC_H_

#include <msp430.h>
#include <stdint.h>

void initADC(void);
void UARTtoADC(void);
void readADC(void);

#endif /* ADC_H_ */
