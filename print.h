/*
 * print.h
 *
 *  Created on: Feb 8, 2019
 *      Author: krad2
 */

#ifndef PRINT_H_
#define PRINT_H_

#include <msp430.h>
#include <stdarg.h>

void putc(unsigned);
void puts(char *);
void sendByte(unsigned char byte);
void print(char *format, ...);

void initUART(void);
void ADCtoUART(void);

#endif /* PRINT_H_ */
