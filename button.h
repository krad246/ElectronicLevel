#ifndef BUTTON_H_
#define BUTTON_H_

#include <dependencies.h>

// Number of unit delays to debounce for
#define THRESHOLD 100

void initButton(void);
void disableButton(void);

#endif /* BUTTON_H_ */
