#ifndef SRC_DEFINES_H_
#define SRC_DEFINES_H_

#include <xil_types.h>
#include <stdbool.h>

//#define DEBUG

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define I2C_SLAVE_DEVICE_ADDRESS 0x3C

#define STATUS_BLINK 200 //ms

#define TIME_TO_US_DIVIDER 325 //XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2000000
#define TIME_TO_US(i) (i / TIME_TO_US_DIVIDER)
#define US_TO_TIME(i) (i * TIME_TO_US_DIVIDER)

enum displayAlignment 
{
    Text_start_left, 
    Text_start_center, 
    Text_start_right
};

enum displayFontSelect 
{
    Font_small, 
    Font_medium, 
	Font_medium_large,
    Font_large
};

typedef struct AudioData {
    s32 left;
    s32 right;
} audioData;

typedef struct FilterData {
    s8 filterAmplitudes[6];
    uint8_t volume;
} filterData;

typedef struct DisplayData {
    char line1[15];
    char line2[15];
    char line3[15];
} displayData;

typedef struct DraaiknopData {
    bool pushed;
    bool right;
    bool left;
} draaiknopData;

#endif
