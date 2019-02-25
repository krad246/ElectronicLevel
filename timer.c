#include "timer.h"

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

// System tick that manages software timers
extern task tasks[NUM_TASKS];
extern uint16_t timers[NUM_TASKS];
extern uint16_t frequencies[NUM_TASKS];

#pragma vector = TIMER1_A0_VECTOR
interrupt void scheduler(void) {
	uint8_t i;
	for (i = 0; i < NUM_TASKS; i++) {
		timers[i]++;

		if (timers[i] > frequencies[i]) {
			task t = tasks[i];
			if (t) t();
			timers[i] = 0;
		}
	}
}
