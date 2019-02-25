#include "kernel.h"

inline void initTask(task t) {
	register struct proc *p = (struct proc *) taskBase;
	p += taskCount;

	p->address = (uint16_t) t;
	p->pid = taskCount;
	taskCount++;
}
