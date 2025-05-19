#ifndef UI_LCD_TASK_H
#define UI_LCD_TASK_H

typedef enum {
  UI_PAGE_INIT,
  UI_PAGE_LANGUAGE_SELECT,
  UI_PAGE_HOME,
  UI_PAGE_MENU,
  UI_PAGE_PIN
} ui_page_t;

typedef struct {
  ui_page_t page;
} lcd_msg_t;

void create_ui_lcd_task(void);

#endif
