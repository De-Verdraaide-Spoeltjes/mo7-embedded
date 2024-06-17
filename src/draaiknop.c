#include "draaiknop.h"
#include "xgpio.h"
#include <stdbool.h>
#include "xtime_l.h"

#include "defines.h"
#include "menucontroller.h"

// Define pins voor rotary encoder
#define ROTARY_ENCODER_A_MASK 0x01
#define ROTARY_ENCODER_B_MASK 0x02
#define ROTARY_ENCODER_BUTTON_MASK 0x04

void readRotaryEncoder(u32 inputValue, draaiknopData* result);

void encoderInterrupt(void* gpio) {
	// Disable interrupts
	XGpio_InterruptDisable((XGpio *)gpio, 2);

	// // Ignore additional inputs
	// if ((XGpio_InterruptGetStatus((XGpio *)gpio) & 2) != 1) {
	// 	return;
	// }

	// Read the input value
	u32 gpioValue = XGpio_DiscreteRead((XGpio *)gpio, 2);

	// Read rotary encoder
    draaiknopData draaiknop;
    draaiknop.left = false;
    draaiknop.right = false;
    draaiknop.pushed = false;
	readRotaryEncoder(gpioValue, &draaiknop);

    xil_printf("Rotary encoder: %d %d %d\n", draaiknop.left, draaiknop.right, draaiknop.pushed);

    // Update the menu
    RunMenuController(&draaiknop);

	// Clear interrupt
	XGpio_InterruptClear((XGpio *)gpio, 2);

	// Re-enable interrupts
	XGpio_InterruptEnable((XGpio *)gpio, 2);
}

void readRotaryEncoder(u32 inputValue, draaiknopData* result)
{
    static bool lastA = false;
    static bool lastB = false;

    // Read the GPIO inputs
    u32 input = inputValue & 0b111;
    bool A = (input & ROTARY_ENCODER_A_MASK) != 0;
    bool B = (input & ROTARY_ENCODER_B_MASK) != 0;
    bool button = (input & ROTARY_ENCODER_BUTTON_MASK) == 0;

    // Determine rotation direction
    if (A && !lastA)
    {
        if (B)
        {
            result->right = true;
            result->left = false;
        } else
        {
            result->right = false;
            result->left = true;
        }
    }

    // Update the last state
    lastA = A;
    lastB = B;

    // Update the button state
    result->pushed = button;
}