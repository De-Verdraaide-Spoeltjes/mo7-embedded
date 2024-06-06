#ifndef SRC_MENUCONTROLLER_H
#define SRC_MENUCONTROLLER_H

#include "defines.h"
#include "XStatus.h"

XStatus InitMenuController(draaiknopData *draaiknop, filterData *filters, displayData *display);
void RunMenuController();
void UpdateDisplay();

#endif
