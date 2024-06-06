#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "defines.h"
#include "audiocodec.h"
#include "audioController.h"
#include "audiofilters.h"
#include "display.h"
#include "draaiknop.h"
#include "menucontroller.h"
#include "volumecontroller.h"
#include "interrupt_controller.h"

#include "sleep.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xtime_l.h"


void statusLED();

#define BTN_DEVICE_ID           XPAR_BTNS_SWITCHES_INPUTS_DEVICE_ID
#define BTN_CHANNEL             1
#define STATUS_LED_DEVICE_ID    XPAR_LEDS_OUTPUTS_DEVICE_ID
XGpio buttonGpio;
XGpio leds;
XScuGic interruptController;

draaiknopData draaiknop;
filterData filters;
displayData display;

void hold(u32 time);

XStatus initButton() {
	XStatus status;
    // Initialize the GPIO instance
    status = XGpio_Initialize(&buttonGpio, BTN_DEVICE_ID);
    if (status != XST_SUCCESS) {
        printf("Error initializing button GPIO\r\n");
        return XST_FAILURE;
    }

    // Set the GPIO channel direction to input
    XGpio_SetDataDirection(&buttonGpio, BTN_CHANNEL, 0xFFFFFFFF);

    return XST_SUCCESS;
}


void Initialize() {
	XGpio_DiscreteWrite(&leds, 1, 0x3);

	XStatus status, init_state = XST_SUCCESS;
	status = initButton();
	if (status != XST_SUCCESS) {
		print("Error initializing buttons\n\r");
	}
	init_state |= status;

	status = InitMenuController(&draaiknop, &filters, &display);
	if (status != XST_SUCCESS) {
		print("Error initializing menu controller\n\r");
	}
	init_state |= status;

	status = initDisplay(&display);
	if (status != XST_SUCCESS) {
		print("Error initializing display\n\r");
	}
	init_state |= status;


	status = initInterruptController(&interruptController);
	if (status != XST_SUCCESS) {
		print("Error initializing interrupt controller\n\r");
	}
	init_state |= status;

	status = initAudioController();
	if (status != XST_SUCCESS) {
		print("Error initializing audio controller\n\r");
	}
	init_state |= status;

	calculateCoefficients();

	status = setupTimerInterrupt(&interruptController, INTERRUPT_PERIOD_US, audioInterruptHandler);
	if (status != XST_SUCCESS) {
		print("Error setting up timer interrupt\n\r");
	}
	init_state |= status;

	if (init_state == XST_SUCCESS) {
		XGpio_DiscreteWrite(&leds, 1, 0x2);
		print("Embedded application initialized\n\r");
	} else {
		XGpio_DiscreteWrite(&leds, 1, 0x1);
		usleep(5000000); // 5 seconds
		Initialize();
	}
}


int main()
{
    init_platform();
	print("Starting embedded application\n\r");

	XGpio_Initialize(&leds, STATUS_LED_DEVICE_ID);
	XGpio_SetDataDirection(&leds, 1, 0x0);
    
    static XTime displayOldTime = 0;

    Initialize();

    usleep(500000);	// 0.5 seconds
	// RunDisplay();

    while (1) {
       statusLED();
       RunMenuController();

        XTime timeNow;
		XTime_GetTime(&timeNow);
        if (timeNow - displayOldTime > US_TO_TIME(1000000 / 30)) {
            displayOldTime = timeNow;
            // RunDisplay();

            u8 buttonData = XGpio_DiscreteRead(&buttonGpio, BTN_CHANNEL);
			draaiknop.left = buttonData & 0x01;
			draaiknop.right = buttonData & 0x02;
			draaiknop.pushed = buttonData & 0x04;

        }
    }

    cleanup_platform();
    return 0;
}

void statusLED() {
    XTime tNow;
    static XTime tOld;
	static bool state;
    XTime_GetTime(&tNow);
    if (tOld + US_TO_TIME(STATUS_BLINK) * 1000 < tNow) {
        // Toggle status LED
        XGpio_DiscreteWrite(&leds, 1, state << 2);
        state = !state;
        tOld = tNow;
    }
}
