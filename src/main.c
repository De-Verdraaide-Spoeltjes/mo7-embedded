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

int main()
{
    init_platform();
    print("Starting up\n\r");

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
