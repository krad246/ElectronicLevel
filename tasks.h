#ifndef TASKS_H_
#define TASKS_H_

#include <dependencies.h>

#define NUM_TASKS 10

typedef void (*task)(void);

typedef struct proc {
	task t;
	uint16_t timer;
	uint16_t freq;
} proc;

void taskSetup(void);
void registerTask(task t, uint8_t pid, uint16_t freq);

#endif /* TASKS_H_ */
