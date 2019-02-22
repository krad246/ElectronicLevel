#include "timer.h"

void initTimer(void) {
	TA0CTL = MC_0 | TACLR;
	TA1CCR0 = 1000;

	TA1CCTL0 = CCIE;

	TA1CTL = MC_1 | TASSEL_2;
}

#pragma vector = TIMER1_A0_VECTOR
interrupt void sysTick() {

}
