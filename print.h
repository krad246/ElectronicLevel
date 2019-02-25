#ifndef PRINT_H_
#define PRINT_H_

#include <dependencies.h>

void putc(unsigned);
void puts(char *);
void sendByte(unsigned char byte);
void print(char *format, ...);

void initUART(void);
void ADCtoUART(void);

#endif /* PRINT_H_ */
