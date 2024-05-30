#ifndef SRC_AUDIOFILTERS_H_
#define SRC_AUDIOFILTERS_H_

#include "xstatus.h"

void interruptHandler(void *CallbackRef);
void audioInterruptHandler(void *CallbackRef);
void runFilters();

#endif