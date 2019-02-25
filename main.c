#include <msp430.h> 
#include <stdint.h>

#include "adc.h"
#include "kernel.h"
#include "math.h"
#include "timer.h"
#include "print.h"
#include "button.h"
#include "sleep.h"

void initPorts(void);

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// Find the base address for task initialization
	taskBase = _get_SP_register();

	// Initialize a sample task;
	initTask(initPorts);

	// Set to 1 MHz clock
	DCOCTL = CALDCO_1MHZ;
	BCSCTL1 = CALBC1_1MHZ;

	// Initialize unused ports
	initPorts();

	// Set up button debouncer
	initButton();

	// Set up ADC registers
	initADC();

	// Initialize the UART for printing
	initUART();

	// Begin calibration
	print("\n\r");
	print("---------------\n\r");
	print("| Calibration |\n\r");
	print("---------------\n\r");
	print("\n\r");


	// Switch to analog mode
	UARTtoADC();

	// Initialize the system tick
	initTimer();

	_low_power_mode_0();

	return 0;
}

// Set pins as high impedance outputs
void initPorts(void) {
	P2DIR = 0xFF;
	P2OUT = 0x00;

	P3DIR = 0xFF;
	P3OUT = 0x00;
}

// Calibration basis vectors
static uint16_t x0, y0, z0;

// Phase of calibration and min / max data
static uint8_t calibState = 0;
static uint16_t calibVals[6] = { 0 };

// Samples array for ADC
extern int16_t samples[3];

// Button callback
inline void outputFunction(void) {
	// If calibration is done, return
	if (calibState > 6) return;

	// If in the middle of calibrating
	if (calibState < 6) {
		// Set the appropriate value in the array
		if (calibState < 2) {
			calibVals[calibState] = samples[2];
		} else if (calibState < 4) {
			calibVals[calibState] = samples[1];
		} else {
			calibVals[calibState] = samples[0];
		}

		// Status update
		print("Got sample: %u\r\n", calibVals[calibState]);
		print("\r\n");

		// Move to next phase
		calibState++;
	}

	// When calibration is done
	if (calibState == 6) {
		// Calculate basis vectors
		x0 = (calibVals[0] + calibVals[1]) >> 1;
		y0 = (calibVals[2] + calibVals[3]) >> 1;
		z0 = (calibVals[4] + calibVals[5]) >> 1;

		// Print out basis vectors
		print("-------\r\n");
		print("x0 = %u\r\n", x0);
		print("y0 = %u\r\n", y0);
		print("z0 = %u\r\n", z0);
		print("-------\r\n");

		print("Starting application...\r\n");

		// Move to next state
		calibState++;
	}
}
