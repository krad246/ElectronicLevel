#include "timer.h"

extern void readADC();
extern void processData();

// Initialize the system tick
void initTimer(void) {
	// Stop and clear the timer
	TA1CTL = MC_0 | TACLR;

	// Set it to 1 ms period
	TA1CCR0 = 1000;

	// Enable interrupts and start the timer
	TA1CCTL0 = CCIE;
	TA1CTL = MC_1 | TASSEL_2;
}

// System tick software timer callback
void manageTimers(void) {

}

uint16_t counter = 0;
extern void print(char *format, ...);
extern void UARTtoADC();
extern void ADCtoUART();

extern uint32_t arr[3];

uint32_t tick = 0;

// System tick that manages software timers
#pragma vector = TIMER1_A0_VECTOR
interrupt void sysTick(void) {
	tick++;

	// Read ADC values
	readADC();

	// Process ADC values
	processData();

	counter++;
	if (counter > 500) {
		counter = 0;
		ADCtoUART();

		print("Time: %n\n\r", tick);
		print("x: %u\n\r", arr[0] >> 3);
		print("y: %u\n\r", arr[1] >> 3);
		print("z: %u\n\r", arr[2] >> 3);
		print("\n\r");

		UARTtoADC();
	}
}
