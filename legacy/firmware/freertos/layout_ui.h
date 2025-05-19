#ifndef __LAYOUT_UI_H__
#define __LAYOUT_UI_H__

#include "stdbool.h"

typedef struct {
    const char *title;
    const char *desc;
    const char *line1;
    const char *line2;
    const char *line3;
    const char *line4;
    const char *btnNo;
    const char *btnYes;
} layout_ui_dialog_t;

void layout_ui_home(void);
void layout_ui_pin_input(char *title, int pos, int index);
void layout_ui_pin_error(bool retry);
void layout_ui_status_bar(void);

#endif
