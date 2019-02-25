#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>

typedef void (*task)(void);

struct __attribute__((__packed__)) proc {
	uint16_t address;
	uint16_t sp;
	uint8_t pid;
};

static uint8_t taskCount = 0;
uint16_t taskBase;
static const uint8_t taskSize = 0;

void initTask(task t);

#endif /* KERNEL_H_ */
