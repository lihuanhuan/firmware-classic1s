#ifndef _MENU_LIST_H_
#define _MENU_LIST_H_

#include <stdbool.h>

void menu_language_init(void);
void main_menu_init(bool state);
void menu_autolock_added_custom(void);
void menu_default(void);

void update_pin_menu_name(bool has_pin);
bool current_menu_is_main(void);

#endif
