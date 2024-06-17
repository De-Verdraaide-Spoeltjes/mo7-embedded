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

#define ENCODER_DEVICE_ID	   	XPAR_ARDUINO_GPIO_DEVICE_ID
#define ENCODER_CHANNEL			2
#define ENCODER_INTERRUPT_ID	XPAR_FABRIC_ARDUINO_GPIO_IP2INTC_IRPT_INTR

#define BTN_DEVICE_ID           XPAR_BTNS_SWITCHES_INPUTS_DEVICE_ID
#define BTN_CHANNEL             1

#define STATUS_LED_DEVICE_ID    XPAR_LEDS_OUTPUTS_DEVICE_ID
#define STATUS_LED_CHANNEL      2

XGpio encoderGpio;
XGpio buttonGpio;
XGpio leds;
XScuGic interruptController;

filterData filters;
displayData display;

void hold(u32 time);
void risingEdge(draaiknopData *input, draaiknopData *output);

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
	XGpio_DiscreteWrite(&leds, STATUS_LED_CHANNEL, LED_ORANGE);

	XStatus status, init_state = XST_SUCCESS;
	status = initButton();
	if (status != XST_SUCCESS) {
		print("Error initializing buttons\n\r");
	}
	init_state |= status;

	status = InitMenuController(&filters, &display);
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

	status = initAudioFilters(&filters);
	if (status != XST_SUCCESS) {
		print("Error initializing audio filters\n\r");
	}

	// status = setupGpioWithInterrupt(&interruptController, &encoderGpio, ENCODER_DEVICE_ID, ENCODER_CHANNEL, ENCODER_INTERRUPT_ID, encoderInterrupt);
	// if (status != XST_SUCCESS) {
	// 	print("Error setting up rotary encoder interrupt\n\r");
	// }
	// init_state |= status;

	status = setupTimerInterrupt(&interruptController, INTERRUPT_PERIOD_US, audioInterruptHandler);
	if (status != XST_SUCCESS) {
		print("Error setting up timer interrupt\n\r");
	}
	init_state |= status;

	if (init_state == XST_SUCCESS) {
		XGpio_DiscreteWrite(&leds, STATUS_LED_CHANNEL, LED_GREEN);
		print("Embedded application initialized\n\r");
	} else {
		XGpio_DiscreteWrite(&leds, STATUS_LED_CHANNEL, LED_RED);
		usleep(5000000); // 5 seconds
		Initialize();
	}
}


int main()
{
	init_platform();
	print("Starting embedded application\n\r");

	XGpio_Initialize(&leds, STATUS_LED_DEVICE_ID);
	XGpio_SetDataDirection(&leds, STATUS_LED_CHANNEL, 0x0);
    
    static XTime displayOldTime = 0;

    Initialize();

    usleep(500000);	// 0.5 seconds

    while (1) {
       statusLED();

        XTime timeNow;
		XTime_GetTime(&timeNow);
        if (timeNow - displayOldTime > US_TO_TIME(1000000 / 30)) {
            displayOldTime = timeNow;
            RunDisplay();

            u8 buttonData = XGpio_DiscreteRead(&buttonGpio, BTN_CHANNEL);
			draaiknopData draaiknop;
			draaiknop.left = buttonData & 0x01;
			draaiknop.right = buttonData & 0x02;
			draaiknop.pushed = buttonData & 0x04;

			draaiknopData draaiknopRisingEdge;
			risingEdge(&draaiknop, &draaiknopRisingEdge);

			RunMenuController(&draaiknopRisingEdge);
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
        XGpio_DiscreteWrite(&leds, STATUS_LED_CHANNEL, state ? LED_BLUE : 0x0);
        state = !state;
        tOld = tNow;
    }
}

void risingEdge(draaiknopData *input, draaiknopData *output) {
	static draaiknopData storage;

	output->left = input->left && !storage.left;
	output->right = input->right && !storage.right;
	output->pushed = input->pushed && !storage.pushed;

	storage = *input;
}
