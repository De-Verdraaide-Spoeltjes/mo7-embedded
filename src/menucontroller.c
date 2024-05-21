#include "menucontroller.h"
#include "defines.h"

#include <stdio.h>

void ButtonPressed();
void ButtonRight();
void ButtonLeft();
void UpdateDisplay();

draaiknopData *DraaiknopData;

filterData *FilterData;

uint8_t menuState = 0;
uint8_t menuItemSelected = 0;

displayData *DisplayData;

XStatus InitMenuController(draaiknopData *draaiknop, filterData *filters, displayData *display) {
    DraaiknopData = draaiknop;
    FilterData = filters;
    DisplayData = display;

    for (int i = 0; i < 8; i++) {
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
    } else if (DraaiknopData->right) {
        ButtonRight();
        DraaiknopData->right = false;
    } else if (DraaiknopData->left) {
        ButtonLeft();
        DraaiknopData->left = false;
    }

    UpdateDisplay();
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
        if (menuItemSelected < 9) {
            menuItemSelected++;
        }
    } else if (menuState > 0 && menuState < 9) {
        // If a filter filters is active, increase the selected filter's value
        if (FilterData->filterAmplitudes[menuState] < 100) {
            FilterData->filterAmplitudes[menuState]++;
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
    } else if (menuState > 0 && menuState < 9) {
        // If a filter filters is active, increase the selected filter's value
        if (FilterData->filterAmplitudes[menuState] > -100) {
            FilterData->filterAmplitudes[menuState]--;
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
        if (menuItemSelected == 9) {
            sprintf(DisplayData->line2, "Volume");
        } else {
            sprintf(DisplayData->line2, "Filter %d", menuItemSelected);
        }
        sprintf(DisplayData->line3, "Click to select");
    } else if (menuState > 0 && menuState < 9) {
        sprintf(DisplayData->line1, "Filter %d", menuState);
        sprintf(DisplayData->line2, "Gain: %d%%", FilterData->filterAmplitudes[menuState]);
        sprintf(DisplayData->line3, "Turn to adjust");
    } else {
        sprintf(DisplayData->line1, "Volume");
        sprintf(DisplayData->line2, "Volume: %d%%", FilterData->volume);
        sprintf(DisplayData->line3, "Turn to adjust");
    }
}
