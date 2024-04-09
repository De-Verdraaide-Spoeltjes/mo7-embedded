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

    print("Hello World\n\r");
    print("Successfully ran Hello World application");
    cleanup_platform();
    return 0;
}
