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
extern proc tasks[NUM_TASKS];

// Round robin scheduler with a notion of time
#pragma vector = TIMER1_A0_VECTOR
interrupt void scheduler(void) {
	// Loop through the task list
	uint8_t i;
	for (i = 0; i < NUM_TASKS; i++) {
		// Increment each software timer
		tasks[i].timer++;

		// Check if it is time to run the function
		if (tasks[i].timer > tasks[i].freq) {
			// If so, run the function and reset the counter
			task t = tasks[i].t;
			if (t) t();
			tasks[i].timer = 0;
		}
	}
}
