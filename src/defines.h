#ifndef SRC_DEFINES_H_
#define SRC_DEFINES_H_

#include <xil_types.h>
#include <stdbool.h>

typedef struct AudioData {
    s32 left;
    s32 right;
} audioData;

typedef struct FilterData {
    s16 filterAmplitudes[6];
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

#define FILTER_INCREMENT 1000
#define VOLUME_INCREMENT 1000

#endif