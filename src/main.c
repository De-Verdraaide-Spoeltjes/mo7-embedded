#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "defines.h"
#include "audiocodec.h"
#include "audiofilters.h"
#include "display.h"
#include "draaiknop.h"
#include "menucontroller.h"
#include "volumecontroller.h"

#include "sleep.h"
#include "xgpio.h"

XGpio buttonGpio;
#define BTN_DEVICE_ID XPAR_BTNS_SWITCHES_INPUTS_DEVICE_ID
#define BTN_CHANNEL 1

int initButton() {
    int status;
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

int main()
{
    init_platform();
    print("Starting up\n\r");

    initButton();

    draaiknopData draaiknop;
    filterData filters;
    displayData display;

    InitMenuController(&draaiknop, &filters, &display);

    while (1) {
        RunMenuController();
    }

    cleanup_platform();
    return 0;
}
