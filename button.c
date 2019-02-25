#include "button.h"

// Sets up button port registers
void initButton(void) {
	// Set BIT3 to input mode
	P1DIR &= ~BIT3;

	// Enable and use the  pullup resistor
	P1REN |= BIT3;
	P1OUT |= BIT3;

/* 	Enable interrupts for the button on the falling edge
 	and clear the interrupt flag */
	P1IES |= BIT3;
	P1IFG &= ~BIT3;
	P1IE |= BIT3;
}

// Button state variable
volatile uint8_t state = 0;

// Debounce state counter
volatile uint8_t count = 0;

// ISR to service button press
#pragma vector = PORT1_VECTOR
interrupt void onButtonPress(void) {
	// Start the debouncing state machine
	count = 0;

	// Capture a snapshot of the button
	state = P1IN & BIT3;

	// Disable button port interrupts
	P1IFG &= ~BIT3;
	P1IE &= ~BIT3;

	// Enable interrupts on the watchdog timer
	IFG1 &= ~WDTIFG;
	IE1 |= WDTIE;

	// Start the watchdog timer for 0.5 ms
	WDTCTL = WDT_MDLY_0_5;
}

// ISR for the debouncing
#pragma vector = WDT_VECTOR
interrupt void debouncer(void) {
/* 	Check if the button state is constant;
  	if so, move to the next snapshot */
	if (state == (P1IN & BIT3)) count++;
	else {
	/* 	Otherwise reset the debounce window
	   	and recapture the button state */
		count = 0;
		state = P1IN & BIT3;
	}

/* 	Check if the button state
	has stayed constant for long enough */
	if (count == THRESHOLD) {
	/* 	Check if the button is still
		being held; if so, perform the output
		function */
		if (state == 0) {
			outputFunction();
		}

		// Stop and clear the watchdog timer
		WDTCTL = WDTPW | WDTHOLD;

		// Disable watchdog timer interrupts
		IE1 &= ~WDTIE;
		IFG1 &= ~WDTIFG;

		// Reenable button interrupts
		P1IE |= BIT3;
	}
}
