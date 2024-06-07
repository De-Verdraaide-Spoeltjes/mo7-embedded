#ifndef SRC_INTERRUPT_CONTROLLER_H
#define SRC_INTERRUPT_CONTROLLER_H

#include "xstatus.h"
#include "xscugic.h"
#include "xgpio.h"

XStatus initInterruptController(XScuGic *interruptController);
XStatus setupGpioWithInterrupt(XScuGic *interruptController, XGpio *gpio, u16 gpioDeviceId, u32 interruptId, Xil_ExceptionHandler handler);
XStatus setupTimerInterrupt(XScuGic *interruptController, uint32_t time, Xil_ExceptionHandler handler);

#endif
