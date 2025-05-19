#include <stdbool.h>

#include "buttons.h"

#include "key_task.h"
#include "task_header.h"
#include "user_messages.h"

typedef enum { BTN_YES, BTN_NO, BTN_UP, BTN_DOWN, BTN_COUNT } button_id_t;

typedef struct {
  uint32_t port;
  uint16_t pin;
  bool high_level;
  uint8_t value;
} button_config_t;

typedef struct {
  bool is_pressed;
  TickType_t last_press_time;
  TickType_t long_press_time;
} button_state_t;

static key_state_t current_key_state = KEY_STATE_UI;

static const button_config_t button_configs[] = {
    {BTN_PORT, BTN_PIN_YES, false, KEY_CONFIRM},
    {BTN_PORT, BTN_PIN_UP, false, KEY_UP},
    {BTN_PORT, BTN_PIN_DOWN, false, KEY_DOWN},
    {BTN_PORT_NO, BTN_PIN_NO, true, KEY_CANCEL},
};

static void key_scan_task(void *pvParameters) {
  (void)pvParameters;

  button_state_t button_states[BTN_COUNT] = {0};
  const TickType_t debounce_time = pdMS_TO_TICKS(10);

  while (1) {
    uint16_t state = buttonRead();

    for (int i = 0; i < BTN_COUNT; i++) {
      button_state_t *button_state = &button_states[i];
      if (button_configs[i].high_level ? (state & button_configs[i].pin)
                                       : !(state & button_configs[i].pin)) {
        if (!button_state->is_pressed) {
          button_state->is_pressed = true;
          button_state->last_press_time = xTaskGetTickCount();
        } else {
          // long press
        }
      } else {
        if (button_state->is_pressed) {
          button_state->is_pressed = false;
          if (xTaskGetTickCount() - button_state->last_press_time >
              debounce_time) {
            key_msg_t msg = {button_configs[i].value, 0};
            if (current_key_state == KEY_STATE_UI) {
              xQueueSend(ui_key_msg_queue, &msg, portMAX_DELAY);
            } else {
              xQueueSend(cmd_key_msg_queue, &msg, portMAX_DELAY);
            }
          }
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

uint8_t key_wait_for_exit(uint32_t timeout) {
  key_msg_t msg;
  if (timeout == 0) {
    xQueueReceive(ui_key_msg_queue, &msg, portMAX_DELAY);
  } else {
    xQueueReceive(ui_key_msg_queue, &msg, pdMS_TO_TICKS(timeout));
  }
  return msg.value;
}

void set_key_state(key_state_t state) {
    current_key_state = state;
}

void create_key_task(void) {
  xTaskCreate(key_scan_task, "key task", 128, NULL, TASK_PRIORITY_HIGH, NULL);
}
