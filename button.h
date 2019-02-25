#ifndef BUTTON_H_
#define BUTTON_H_

#include <dependencies.h>

// Number of unit delays to debounce for
#define THRESHOLD 50

void initButton(void);
extern inline void outputFunction(void);

#endif /* BUTTON_H_ */
