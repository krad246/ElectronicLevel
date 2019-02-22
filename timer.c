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

extern int16_t arr[3];

// System tick that manages software timers
#pragma vector = TIMER1_A0_VECTOR
interrupt void sysTick(void) {
	// Read ADC values
	readADC();

	// Process ADC values
	processData();

	counter++;
	if (counter > 500) {
		ADCtoUART();
		counter = 0;
		print("\r\n");
		print("%u, %u, %u \r\n", arr[0], arr[1], arr[2]);
		print("\r\n");
		UARTtoADC();
	}
}
