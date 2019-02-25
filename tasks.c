#include "tasks.h"

// List of tasks to run
proc tasks[NUM_TASKS];

// Data processing task
extern void processData(void);

// Computing angles task
extern void getAngles(void);

// System timer
uint32_t tick = 0;
inline void sysTick(void) {
	tick++;
}

// Array of filtered samples
extern _q15 arr[3];

// Print function and pin change functions
extern void UARTtoADC();
extern void ADCtoUART();
extern void print(char *format, ...);

// Basis vectors for logging
extern _q15 x0, y0, z0;

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
	print("\n\r");

	// Switch back to analog read mode
	UARTtoADC();
}

// Function to set up tasks
void taskSetup(void) {
	registerTask(sysTick, 0, 1000);
	registerTask(processData, 1, 5);
	registerTask(printReadings, 2, 1000);
	registerTask(getAngles, 3, 17);
}

// Initializes a task in the proc table
void registerTask(task t, uint8_t pid, uint16_t freq) {
	tasks[pid] = (proc) { t, 0, freq };
}
