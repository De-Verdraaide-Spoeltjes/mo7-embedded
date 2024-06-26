#ifndef SRC_DEFINES_H_
#define SRC_DEFINES_H_

#include <xil_types.h>
#include <stdbool.h>

// #define DEBUG
// #define PRINT_DURATION
// #define AUTO_RECALCULATE_COEFFICIENTS

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define I2C_SLAVE_DEVICE_ADDRESS 0x3C
#define MAX_TEXT_LENGTH 16

#define STATUS_BLINK 200 //ms

#define INTERRUPT_PERIOD_US 20 //us
#define SAMPLE_RATE 	48000U

#define IIC_CLK 100000

#define DEBOUNCE_TIME 50

#define TIME_TO_US_DIVIDER 325 //XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2000000
#define TIME_TO_US(i) (i / TIME_TO_US_DIVIDER)
#define US_TO_TIME(i) (i * TIME_TO_US_DIVIDER)

#define BIGGEST(a, b) ((a) > (b) ? (a) : (b))

enum displayAlignment {
    Text_start_left, 
    Text_start_center, 
    Text_start_right
};

enum displayFontSelect {
    Font_small, 
    Font_medium, 
	Font_medium_large,
    Font_large
};

enum filterSelect {
    Filter_lowpass,
    Filter_upper_mids, 
    Filter_presence,
    Filter_brilliance,
    Filter_open_air,
    FILTER_COUNT
};

enum ledColor {
    LED_OFF = 0,
    LED_RED = 0b100,
    LED_GREEN = 0b010,
    LED_BLUE = 0b001,
    LED_ORANGE = LED_RED | LED_GREEN,
    LED_PURPLE = LED_RED | LED_BLUE,
    LED_CYAN = LED_GREEN | LED_BLUE,
    LED_WHITE = LED_RED | LED_GREEN | LED_BLUE
};

typedef struct AudioData {
    s32 left;
    s32 right;
} audioData;

typedef struct FilterData {
    s8 filterAmplitudes[FILTER_COUNT];
    uint8_t volume;
} filterData;

typedef struct DisplayData {
    char line1[MAX_TEXT_LENGTH];
    char line2[MAX_TEXT_LENGTH];
    char line3[MAX_TEXT_LENGTH];
} displayData;

typedef struct DraaiknopData {
    bool pushed;
    bool right;
    bool left;
} draaiknopData;

#endif
