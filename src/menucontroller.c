#include "menucontroller.h"
#include "defines.h"
#include "audiofilters.h"

#include <stdio.h>

void ButtonPressed();
void ButtonRight();
void ButtonLeft();
void UpdateDisplay();

draaiknopData *DraaiknopData;

filterData *FilterData;

const char filterNames[FILTER_COUNT][MAX_TEXT_LENGTH] = {
    [Filter_lowpass] = "Lowpass",
    [Filter_upper_mids] = "Upper Mids",
    [Filter_presence] = "Presence",
    [Filter_brilliance] = "Brilliance",
    [Filter_open_air] = "Open Air"
};

uint8_t menuState = 0;
uint8_t menuItemSelected = 1;

displayData *DisplayData;

XStatus InitMenuController(draaiknopData *draaiknop, filterData *filters, displayData *display) {
    DraaiknopData = draaiknop;
    FilterData = filters;
    DisplayData = display;

    for (int i = 0; i < FILTER_COUNT; i++) {
        FilterData->filterAmplitudes[i] = 0;
    }
    FilterData->volume = 100;
    
    UpdateDisplay();

    return XST_SUCCESS;
}

void RunMenuController() {
    // Read the state of the rotary encoder
    // Reset the rotary encoder state if an action is performed
    if (DraaiknopData->pushed) {
        ButtonPressed();
        DraaiknopData->pushed = false;
        UpdateDisplay();
    } 
    else if (DraaiknopData->right) {
        ButtonRight();
        DraaiknopData->right = false;
        UpdateDisplay();
    } 
    else if (DraaiknopData->left) {
        ButtonLeft();
        DraaiknopData->left = false;
        UpdateDisplay();
    }
}

void ButtonPressed() {
    if (menuState == 0) {
        // If the main filters is active, enter the selected filters item
        menuState = menuItemSelected;
    } else {
        // If a submenu is active, return to the main filters
        menuState = 0;
    }
}

void ButtonRight() {
    if (menuState == 0) {
        // If the main filters is active, scroll to the next filters item
        if (menuItemSelected < FILTER_COUNT + 1) {
            menuItemSelected++;
        }
    } else if (menuState > 0 && menuState < FILTER_COUNT + 1) {
        // If a filter filters is active, increase the selected filter's value
        if (FilterData->filterAmplitudes[menuState - 1] < 100) {
            FilterData->filterAmplitudes[menuState - 1]++;
        }
    } else {
        // If the volume filters is active, increase the volume
        if (FilterData->volume < 100) {
            FilterData->volume++;
        }
    }
}

void ButtonLeft() {
    if (menuState == 0) {
        // If the main filters is active, scroll to the previous filters item
        if (menuItemSelected > 1) {
            menuItemSelected--;
        }
    } else if (menuState > 0 && menuState < FILTER_COUNT + 1) {
        // If a filter filters is active, increase the selected filter's value
        if (FilterData->filterAmplitudes[menuState - 1] > -100) {
            FilterData->filterAmplitudes[menuState - 1]--;
        }
    } else {
        // If the volume filters is active, increase the volume
        if (FilterData->volume > 0) {
            FilterData->volume--;
        }
    }
}

void UpdateDisplay() {
    if (menuState == 0) {
        sprintf(DisplayData->line1, "Main filters");
        if (menuItemSelected == FILTER_COUNT + 1) {
            sprintf(DisplayData->line2, "Volume");
        } else {
            sprintf(DisplayData->line2, "%s", filterNames[menuItemSelected - 1]);
        }
        sprintf(DisplayData->line3, "Click to select");
    } else if (menuState > 0 && menuState < FILTER_COUNT + 1) {
        sprintf(DisplayData->line1, "%s", filterNames[menuState - 1]);
        sprintf(DisplayData->line2, "Gain: %d%%", FilterData->filterAmplitudes[menuState - 1]);
        sprintf(DisplayData->line3, "Turn to adjust");
    } else {
        sprintf(DisplayData->line1, "Main volume");
        sprintf(DisplayData->line2, "Volume: %d%%", FilterData->volume);
        sprintf(DisplayData->line3, "Turn to adjust");
    }

    // Calculate the new filter coefficients beqause the filter settings have changed
    calculateCoefficients();
}
