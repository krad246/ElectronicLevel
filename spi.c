#include "spi.h"

// Initializes hardware SPI
void initSPI() {
	// Hold the state machine in reset mode
	UCB0CTL1 |= UCSWRST;

	// Set positive edge clocking, send MSB first, set as master, use 3-pin SPI mode
	UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;

	// Use the SMCLK
	UCB0CTL1 |= UCSSEL_2;

	// Use 1x prescaling
	UCB0BR0 = 0x01;
	UCB0BR1 = 0;

	// Default initialize the transmit buffer
	UCB0TXBUF = 0;

	// Set pin 5 to output and enable SPI pin functionality
	P1DIR |= BIT5;
	P1SEL |= BIT5;
	P1SEL2 |= BIT5;

	// Set pin 7 to output and enable SPI pin functionality
	P1DIR |= BIT7;
	P1SEL |= BIT7;
	P1SEL2 |= BIT7;

	// Release the reset state
	UCB0CTL1 &= ~UCSWRST;

	// Enable interrupts
	IE2 |= UCB0TXIE;
}

// Write the byte to the TX buffer and latch the data
inline void send(uint8_t byteValue) {
	// Set the TX buffer and wait for the transaction to complete
	UCB0TXBUF = byteValue;
}

#pragma vector = USCIAB0TX_VECTOR
interrupt void latch(void) {
	// Latch the LED state
	P2OUT |= BIT0;
	P2OUT &= ~BIT0;

	// Clear the interrupt flag
	IFG2 &= ~UCB0TXIFG;
}
