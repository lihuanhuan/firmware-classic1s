#include "user_messages.h"

#include "key_task.h"
#include "queue.h"
#include "ui_lcd_task.h"

QueueHandle_t ui_lcd_msg_queue;
QueueHandle_t key_msg_queue;
SemaphoreHandle_t system_state_semaphore;

void user_messages_init(void) {
  ui_lcd_msg_queue = xQueueCreate(3, sizeof(lcd_msg_t));
  key_msg_queue = xQueueCreate(3, sizeof(key_msg_t));
  system_state_semaphore = xSemaphoreCreateMutex();
}
