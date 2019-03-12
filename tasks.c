#include "tasks.h"

// List of tasks to run
proc tasks[NUM_TASKS];
uint8_t pid = 0;

// ADC read task
extern inline void readADC(void);

// Angle computation task
extern inline void getOrientation(void);

// System timer
uint32_t tick = 0;
inline void sysTick(void) {
	tick++;
}

// Array of filtered samples
extern _q15 arr[3];

// Print function and pin change functions
extern inline void UARTtoADC();
extern inline void ADCtoUART();
extern void print(char *format, ...);

// Basis vectors for logging
extern _q15 x0, y0, z0;
extern _q15 theta, phi;

// Logging callback
inline void printReadings(void) {
	// Switch to UART mode
	ADCtoUART();

	// Print a banner with the readings
	const _q15 dx = arr[0] - x0;
	const _q15 dy = arr[1] - y0;
	const _q15 dz = arr[2] - z0;

	print("\n\r");
	print("------------\n\r");
	print("| Time: %n |\n\r", tick);
	print("------------\n\r");
	print("dx: %i\n\r", dx);
	print("dy: %i\n\r", dy);
	print("dz: %i\n\r", dz);
	print("theta: %i\n\r", theta);
	print("phi: %i\n\r", phi);
	print("\n\r");

	// Switch back to analog read mode
	UARTtoADC();
}

extern inline void updateOnTheta(void);
extern inline void updateOnPhi(void);
extern inline void updateTicks(void);
extern inline void display(void);

// Function to set up tasks
void taskSetup(void) {
	registerTask(sysTick, 1000);
	registerTask(readADC, 2);
}

// Initializes a task in the proc table
void registerTask(task t, uint16_t freq) {
	tasks[pid] = (proc) { t, 0, freq };
	pid++;
}
