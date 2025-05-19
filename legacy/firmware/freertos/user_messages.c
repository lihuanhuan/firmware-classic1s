#include "user_messages.h"

#include "key_task.h"
#include "queue.h"
#include "layout_ui.h"
#include "ui_lcd_task.h"

QueueHandle_t ui_msg_queue;
QueueHandle_t ui_key_msg_queue, cmd_key_msg_queue;
QueueHandle_t fido_msg_queue;
SemaphoreHandle_t system_state_semaphore;

void user_messages_init(void) {
  ui_msg_queue = xQueueCreate(1, sizeof(ui_msg_t));
  ui_key_msg_queue = xQueueCreate(1, sizeof(key_msg_t));
  cmd_key_msg_queue = xQueueCreate(1, sizeof(key_msg_t));
  fido_msg_queue = xQueueCreate(1, sizeof(void *));
  system_state_semaphore = xSemaphoreCreateMutex();

}
