#include "tasks.h"

task tasks[NUM_TASKS] = { 0 };
uint16_t timers[NUM_TASKS] = { 0 };
uint16_t frequencies[NUM_TASKS];

extern void processData(void);

uint32_t tick = 0;
inline void sysTick(void) {
	tick++;
}

extern uint32_t arr[3];
extern void UARTtoADC();
extern void ADCtoUART();
extern void print(char *format, ...);

inline void printReadings(void) {
	ADCtoUART();

	print("\n\r");
	print("------------\n\r");
	print("| Time: %n |\n\r", tick);
	print("------------\n\r");
	print("x: %u\n\r", arr[0] >> 3);
	print("y: %u\n\r", arr[1] >> 3);
	print("z: %u\n\r", arr[2] >> 3);
	print("\n\r");

	UARTtoADC();
}

void taskSetup(void) {
	registerTask(sysTick, 0, 1000);
	registerTask(processData, 1, 1);
	registerTask(printReadings, 2, 1000);
}

void registerTask(task t, uint8_t pid, uint16_t freq) {
	tasks[pid] = t;
	frequencies[pid] = freq;
}
