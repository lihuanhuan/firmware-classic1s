#ifndef __LAYOUT_UI_H__
#define __LAYOUT_UI_H__

#include "stdbool.h"

void layout_ui_home(void);
void layout_ui_pin_input(char *title, int pos, int index);
void layout_ui_pin_error(bool retry);
void layout_ui_status_bar(void);

#endif
