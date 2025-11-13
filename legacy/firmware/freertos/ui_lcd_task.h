#ifndef UI_LCD_TASK_H
#define UI_LCD_TASK_H

#include "layout_ui.h"

typedef enum {
  UI_PAGE_INIT,
  UI_PAGE_LANGUAGE_SELECT,
  UI_PAGE_HOME,
  UI_PAGE_MENU,
  UI_PAGE_PIN,
  UI_PAGE_CMD,
  UI_PAGE_RESET,
  UI_PAGE_TRANSPORT_BLE,
  UI_PAGE_TRANSPORT_USB,
} ui_page_t;

typedef enum {
  PIN_OPERATION_VERIFY,
  PIN_OPERATION_SET,
} pin_operation_t;

typedef enum {
  UI_MSG_HOME,
} ui_msg_type_t;

typedef struct {
  ui_msg_type_t msg_type;
  layout_ui_dialog_t dialog;
} ui_msg_t;

void layout_set_home(void);

void create_ui_lcd_task(void);
void create_pin_task(pin_operation_t operation);
void create_gen_seed_task(void);
#endif
