#include "tasks.h"
#include "adc.h"
#include "math.h"
#include "timer.h"
#include "print.h"
#include "button.h"
#include "led.h"
#include "spi.h"

void initPorts(void);

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// Set to 8 MHz clock
	DCOCTL = CALDCO_8MHZ;
	BCSCTL1 = CALBC1_8MHZ;

	// Initialize unused ports
	initPorts();

	// Set up button debouncer
	initButton();

	// Set up ADC registers
	initADC();

	// Initialize the UART for printing
	initUART();

	// Initialize the LEDs
	initLEDs();

	// Initialize the SPI for LED matrix
	initSPI();

    // Set the default value for the LED
    send(128);

	// Begin calibration
	print("---------------\n\r");
	print("| Calibration |\n\r");
	print("---------------\n\r");

	// Switch to analog mode
	UARTtoADC();

	// Register the tasks for program execution
	taskSetup();

	// Initialize the scheduler
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
_q15 x0, y0, z0;

// Phase of calibration and min / max data
static volatile uint8_t calibState = 0;
static _q15 calibVals[6] = { 0 };
static uint8_t calibrationDirections[7] = {
	128, 8,
	2, 32,
	128 | 8 | 2 | 32,
	16 | 64 | 4 | 1,
	0
};

extern inline void printReadings(void);

// Samples array for ADC
extern _q15 arr[3];

// Button callback
inline void buttonCallback(void) {
    ADCtoUART();
	// If in the middle of calibrating
	if (calibState < 6) {
		// Set the appropriate value in the array
		if (calibState < 2) {
			calibVals[calibState] = arr[0];
		} else if (calibState < 4) {
			calibVals[calibState] = arr[1];
		} else {
			calibVals[calibState] = arr[2];
		}

		// Status update
		print("Got sample: %u\r\n", calibVals[calibState]);

		// Move to next phase
	    calibState++;
		send(calibrationDirections[calibState]);
	}

	// When calibration is done
	if (calibState == 6) {
		// Calculate basis vectors
		x0 = (calibVals[0] + calibVals[1]) >> 1;
		y0 = (calibVals[2] + calibVals[3]) >> 1;
		z0 = (calibVals[4] + calibVals[5]) >> 1;

		// Print out basis vectors
		print("-------\r\n");
		print("x0 = %i\r\n", x0);
		print("y0 = %i\r\n", y0);
		print("z0 = %i\r\n", z0);
		print("-------\r\n");

		print("Starting application...\r\n");

		// Add all of the display tasks
		// Add the system print
		registerTask(printReadings, 1000);

		// Add the angle calculations
		registerTask(getOrientation, 4);

		// Add the update functions
		registerTask(updateOnTheta, 8);
		registerTask(updateOnPhi, 8);
		registerTask(updateTicks, 1);

		// Add the visual display function
		registerTask(display, 1);

		// Disable the button
		disableButton();
	}
    UARTtoADC();
}
