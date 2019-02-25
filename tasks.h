#ifndef TASKS_H_
#define TASKS_H_

#include <stdint.h>

#define NUM_TASKS 3

typedef void (*task)(void);

void taskSetup(void);
void registerTask(task t, uint8_t pid, uint16_t freq);

#endif /* TASKS_H_ */
