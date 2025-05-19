#include "ui_lcd_task.h"
#include "key_task.h"
#include "task_header.h"
#include "user_messages.h"

#include "buttons.h"
#include "common.h"
#include "config.h"
#include "gettext.h"
#include "rng.h"
#include "secbool.h"

#include "menu_core.h"

#include "layout_ui.h"

typedef enum {
  PIN_OPERATION_VERIFY,
  PIN_OPERATION_SET,
} pin_operation_t;

typedef struct {
  pin_operation_t operation;
} pin_task_param_t;

static int g_system_state = UI_PAGE_INIT;

static TaskHandle_t pin_task_handle = NULL;
static TaskHandle_t menu_task_handle = NULL;
int get_system_state(void) {
  xSemaphoreTake(system_state_semaphore, portMAX_DELAY);
  int state = g_system_state;
  xSemaphoreGive(system_state_semaphore);
  return state;
}

void set_system_state(int state) {
  xSemaphoreTake(system_state_semaphore, portMAX_DELAY);
  g_system_state = state;
  xSemaphoreGive(system_state_semaphore);
}

int generate_random_pin(void) {
  int index = 0;
  do {
    index = random_uniform(10);
  } while (index == 0);
  return index;
}

static bool pin_input(char *prompt, int min_len, int max_len, char *pin) {
  int pos = 0;
  int index = generate_random_pin();
  int max_index = 0;
  key_msg_t msg;

  while (1) {
    layout_ui_pin_input(prompt, pos, index);
    max_index = pos >= min_len ? 10 : 9;
    if (xQueueReceive(key_msg_queue, &msg, portMAX_DELAY) == pdPASS) {
      if (msg.value == KEY_CANCEL) {
        if (pos > 0) {
          pos--;
          pin[pos] = 0;
          index = generate_random_pin();
        } else {
          return false;
        }
      } else if (msg.value == KEY_CONFIRM) {
        if (index == 10) {
          return true;
        }
        pin[pos++] = index + '0';
        if (pos == max_len) {
          return true;
        }
        index = generate_random_pin();
      } else if (msg.value == KEY_UP) {
        index = index > 1 ? index - 1 : max_index;
      } else if (msg.value == KEY_DOWN) {
        index = index < max_index ? index + 1 : 1;
      }
    }
  }
}

static void pin_task(void *pvParameters) {
  pin_task_param_t *pin_info = (pin_task_param_t *)pvParameters;
  char pin[10] = "";
  char pin_confirm[10] = "";

  set_system_state(UI_PAGE_PIN);

  if (pin_info->operation == PIN_OPERATION_VERIFY) {
    if (pin_input(_(T__ENTER_PIN), MIN_PIN_LEN, MAX_PIN_LEN, pin)) {
      bool ret = config_unlock(pin);
      if (!ret) {
        layout_ui_pin_error(false);
      }
    }
  } else {
    if (pin_input(_(T__ENTER_NEW_PIN), DEFAULT_PIN_LEN, MAX_PIN_LEN, pin)) {
      if (pin_input(_(T__ENTER_NEW_PIN_AGAIN), DEFAULT_PIN_LEN, MAX_PIN_LEN,
                    pin_confirm)) {
        if (strcmp(pin, pin_confirm) == 0) {
        }
      }
    }
  }

  memset(pin, 0, sizeof(pin));
  memset(pin_confirm, 0, sizeof(pin_confirm));
  layout_ui_home();
  set_system_state(UI_PAGE_HOME);
  pin_task_handle = NULL;
  vTaskDelete(NULL);
}

void menu_task(void *pvParameters) {
  (void)pvParameters;
  key_msg_t msg;
  while (1) {
    menu_display_refresh();
    if (xQueueReceive(key_msg_queue, &msg, portMAX_DELAY) == pdPASS) {
      if (msg.value == KEY_CANCEL) {
        if (menu_exit()) {
          break;
        }
      } else if (msg.value == KEY_CONFIRM) {
        menu_enter();
      } else if (msg.value == KEY_UP) {
        menu_up();
      } else if (msg.value == KEY_DOWN) {
        menu_down();
      }
    }
  }
  layout_ui_home();
  set_system_state(UI_PAGE_HOME);
  menu_task_handle = NULL;
  vTaskDelete(NULL);
}

static void ui_status_bar_task(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    TickType_t last_update = xTaskGetTickCount();
    layout_ui_status_bar();
    vTaskDelayUntil(&last_update, pdMS_TO_TICKS(1000));
  }
}

static void ui_lcd_task(void *pvParameters) {
  (void)pvParameters;

  // if (!config_isInitialized()) {
  //   layout_language_set(KEY_NULL);
  //   current_page = UI_PAGE_LANGUAGE_SELECT;
  // } else {
  //   layoutHome();
  //   current_page = UI_PAGE_HOME;
  // }

  layout_ui_home();

  set_system_state(UI_PAGE_HOME);

  while (1) {
    key_msg_t msg;
    if (xQueueReceive(key_msg_queue, &msg, portMAX_DELAY) == pdPASS) {
      int state = get_system_state();
      switch (state) {
        case UI_PAGE_HOME:
          if (msg.value == KEY_CANCEL) {
            break;
          }
          if (!session_isUnlocked() && config_hasPin()) {
            if (pin_task_handle == NULL) {
              pin_task_param_t data = {.operation = PIN_OPERATION_VERIFY};
              if (xTaskCreate(pin_task, "pin task", 1024, &data,
                              TASK_PRIORITY_HIGHEST,
                              &pin_task_handle) != pdPASS) {
                ensure(false, "pin_task create failed");
              }
            }
          } else {
            if (menu_task_handle == NULL) {
              if (xTaskCreate(menu_task, "menu task", 1024, NULL,
                              TASK_PRIORITY_HIGHEST,
                              &menu_task_handle) != pdPASS) {
                ensure(false, "menu_task create failed");
              }
            }
          }
          break;
        default:
          break;
      }
    }
  }
}

void create_ui_lcd_task(void) {
  xTaskCreate(ui_lcd_task, "lcd task", 1024, NULL, TASK_PRIORITY_HIGH, NULL);
  xTaskCreate(ui_status_bar_task, "status bar task", 256, NULL,
              TASK_PRIORITY_MEDIUM, NULL);
}
