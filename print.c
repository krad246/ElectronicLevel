/*
 * print.c
 *
 *  Created on: Feb 8, 2019
 *      Author: krad2
 */

#include "print.h"

// Initializes UART
inline void initUART(void) {
	// Reset the UART
	UCA0CTL1 |= UCSWRST;

	// Set the pin functionality to UART
	P1SEL = BIT1 | BIT2;
	P1SEL2 = BIT1 | BIT2;

	// Use the SMCLK
	UCA0CTL1 |= UCSSEL_3;

	// Set the baud rate to 9600
	UCA0BR0 = 0x68;
	UCA0BR1 = 0x00;

	// Use modulation type 3
	UCA0MCTL = UCBRS0;

	// Enable USCI_A0 RX interrupt
	IE2 |= UCA0RXIE;

	// Start UART
	UCA0CTL1 &= ~UCSWRST;
}

/**
 * puts() is used by printf() to display or send a string. This function
 *     determines where printf prints to. For this case it sends a string
 *     out over UART, another option could be to display the string on an
 *     LCD display.
 **/
void puts(char *s) {
	char c;

	// Loops through each character in string 's'
	while (c = *s++) {
		sendByte(c);
	}
}
/**
 * puts() is used by printf() to display or send a character. This function
 *     determines where printf prints to. For this case it sends a character
 *     out over UART.
 **/
void putc(unsigned b) {
	sendByte(b);
}

/**
 * Sends a single byte out through UART
 **/
void sendByte(unsigned char byte)
{
	// USCI_A0 TX buffer ready?
	while (!(IFG2 & UCA0TXIFG));

	// TX -> RXed character
	UCA0TXBUF = byte;
}



/**
 * Interrupt routine for receiving a character over UART
 **/
#pragma vector = USCIAB0RX_VECTOR
interrupt void USCI0RX_ISR(void)
{
	char r = UCA0RXBUF;					// Get the received character
}


static const unsigned long dv[] = {
//  4294967296      // 32 bit unsigned max
		1000000000,// +0
		100000000, // +1
		10000000, // +2
		1000000, // +3
		100000, // +4
//       65535      // 16 bit unsigned max
		10000, // +5
		1000, // +6
		100, // +7
		10, // +8
		1, // +9
		};

static void xtoa(unsigned long x, const unsigned long *dp) {
	char c;
	unsigned long d;
	if (x) {
		while (x < *dp)
			++dp;
		do {
			d = *dp++;
			c = '0';
			while (x >= d)
				++c, x -= d;
			putc(c);
		} while (!(d & 1));
	} else
		putc('0');
}

static void puth(unsigned n) {
	static const char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8',
			'9', 'A', 'B', 'C', 'D', 'E', 'F' };
	putc(hex[n & 15]);
}

void print(char *format, ...)
{
	char c;
	int i;
	long n;

	va_list a;
	va_start(a, format);
	while(c = *format++) {
		if(c == '%') {
			switch(c = *format++) {
				case 's': // String
					puts(va_arg(a, char*));
					break;
				case 'c':// Char
					putc(va_arg(a, char));
				break;
				case 'i':// 16 bit Integer
				case 'u':// 16 bit Unsigned
					i = va_arg(a, int);
					if(c == 'i' && i < 0) i = -i, putc('-');
					xtoa((unsigned)i, dv + 5);
				break;
				case 'l':// 32 bit Long
				case 'n':// 32 bit uNsigned loNg
					n = va_arg(a, long);
					if(c == 'l' && n < 0) n = -n, putc('-');
					xtoa((unsigned long)n, dv);
				break;
				case 'x':// 16 bit heXadecimal
					i = va_arg(a, int);
					puth(i >> 12);
					puth(i >> 8);
					puth(i >> 4);
					puth(i);
				break;
				case 0: return;
				default: goto bad_fmt;
			}
		} else
			bad_fmt: putc(c);
	}
	va_end(a);
}

inline void ADCtoUART(void) {
	// Set pins to digital mode
	ADC10AE0 = 0x00;

	// Reset pin mode to UART
	P1SEL = BIT1 | BIT2;				// P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 | BIT2;				// P1.1 = RXD, P1.2=TXD
}
