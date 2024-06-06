#ifndef SRC_AUDIOFILTERS_H_
#define SRC_AUDIOFILTERS_H_

#include "defines.h"
#include "xstatus.h"

XStatus initAudioFilters(filterData *filters);
void calculateCoefficients();
void interruptHandler(void *CallbackRef);
void audioInterruptHandler(void *CallbackRef);
void runFilters();

#endif