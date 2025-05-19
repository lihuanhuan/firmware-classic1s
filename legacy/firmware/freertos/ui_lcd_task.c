#include "ui_lcd_task.h"
#include "key_task.h"
#include "task_header.h"
#include "user_messages.h"

#include "buttons.h"
#include "common.h"
#include "config.h"
#include "gettext.h"
#include "menu_core.h"
#include "rng.h"
#include "se_chip.h"
#include "secbool.h"
#include "usart.h"

typedef struct {
  pin_operation_t operation;
} pin_task_param_t;

static int g_system_state = UI_PAGE_INIT;

static TaskHandle_t pin_task_handle = NULL;
static TaskHandle_t menu_task_handle = NULL;
static TaskHandle_t gen_seed_task_handle = NULL;

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  (void)xTask;
  uart_printf("Stack overflow detected for task [%s]\n", pcTaskName);
}

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
    if (xQueueReceive(ui_key_msg_queue, &msg, pdMS_TO_TICKS(1000 * 60 * 1)) ==
        pdPASS) {
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
    } else {
      return false;
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
  layout_set_home();
  
  pin_task_handle = NULL;
  vTaskDelete(NULL);
}

static void gen_fido_seed_task(void *pvParameters) {
  (void)pvParameters;
  uint8_t percent = 0;
  UI_WAIT_CALLBACK ui_callback = se_get_ui_callback();
  while (1) {
    secbool ret = se_gen_root_node(&percent);
    if (ret) {
      if (percent == 100) {
        se_fido_set_seed_cached(true);
        break;
      }
      if (ui_callback) {
        ui_callback(_(C__PROCESSING_ETC), percent * 10);
      }
    } else {
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  gen_seed_task_handle = NULL;
  vTaskDelete(NULL);
}

void menu_task(void *pvParameters) {
  (void)pvParameters;
  key_msg_t msg;
  while (1) {
    menu_display_refresh();
    if (xQueueReceive(ui_key_msg_queue, &msg, portMAX_DELAY) == pdPASS) {
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

void layout_set_home(void) {
  ui_msg_t ui_msg = {
    .msg_type = UI_MSG_HOME,
    .dialog = NULL,
  };
  set_system_state(UI_PAGE_HOME);
  xQueueOverwrite(ui_msg_queue, &ui_msg);
}

static void ui_lcd_task(void *pvParameters) {
  (void)pvParameters;

  layout_ui_home();

  set_system_state(UI_PAGE_HOME);
  ui_msg_t ui_msg;
  key_msg_t key_msg;

  while (1) {
    if (xQueueReceive(ui_key_msg_queue, &key_msg, pdMS_TO_TICKS(5)) == pdPASS) {
      {
        int state = get_system_state();
        switch (state) {
          // case UI_PAGE_HOME:
          //   if (key_msg.value == KEY_CANCEL) {
          //     break;
          //   }
          //   if (!session_isUnlocked() && config_hasPin()) {
          //     create_pin_task(PIN_OPERATION_VERIFY);
          //   } else {
          //     if (menu_task_handle == NULL) {
          //       if (xTaskCreate(menu_task, "menu task", 1024, NULL,
          //                       TASK_PRIORITY_HIGHEST,
          //                       &menu_task_handle) != pdPASS) {
          //         ensure(false, "menu_task create failed");
          //       }
          //     }
          //   }
          //   break;
          default:
            break;
        }
      }
    }
    if (xQueueReceive(ui_msg_queue, &ui_msg, pdMS_TO_TICKS(5)) == pdPASS) {
      if (ui_msg.msg_type == UI_MSG_HOME) {
        uart_printf("UI_MSG_HOME\n");
        layout_ui_home();
      }
    }
  }
}

void create_ui_lcd_task(void) {
  xTaskCreate(ui_lcd_task, "lcd task", 1024, NULL, TASK_PRIORITY_HIGHEST, NULL);
  xTaskCreate(ui_status_bar_task, "status bar task", 1024, NULL,
              TASK_PRIORITY_MEDIUM, NULL);
}

void create_pin_task(pin_operation_t operation) {
  if (pin_task_handle == NULL) {
    static pin_task_param_t data;
    data.operation = operation;
    if (xTaskCreate(pin_task, "pin task", 1024, &data, TASK_PRIORITY_HIGHEST,
                    &pin_task_handle) != pdPASS) {
      ensure(false, "pin_task create failed");
    }
  }
}

void create_gen_seed_task(void) {
  if (gen_seed_task_handle == NULL) {
    if (xTaskCreate(gen_fido_seed_task, "gen fido seed task", 256, NULL,
                    TASK_PRIORITY_HIGHEST, &gen_seed_task_handle) != pdPASS) {
      ensure(false, "gen_seed_task create failed");
    }
  }
}
