#include <msp430.h> 
#include <stdint.h>

#include "print.h"
#include "button.h"
#include "sleep.h"

void initPorts(void);

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	initPorts();
	initButton();
	initUART();

	_low_power_mode_0();

	return 0;
}

void initPorts(void) {
	P1DIR |= (BIT0 | BIT6);

	P2DIR = 0xFF;
	P2OUT = 0x00;

	P3DIR = 0xFF;
	P3OUT = 0x00;
}

inline void outputFunction(void) {

}
