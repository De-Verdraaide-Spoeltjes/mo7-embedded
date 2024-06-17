#ifndef SRC_MENUCONTROLLER_H
#define SRC_MENUCONTROLLER_H

#include "defines.h"
#include "XStatus.h"

XStatus InitMenuController(filterData *filters, displayData *display);
void RunMenuController(draaiknopData *draaiknop);
void UpdateDisplay();

#endif
