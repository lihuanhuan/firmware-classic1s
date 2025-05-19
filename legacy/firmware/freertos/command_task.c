#include "command_task.h"
#include "task_header.h"
#include "user_messages.h"

#include "usb.h"

static void usb_task(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    usb_poll();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void create_command_task(void) {
  xTaskCreate(usb_task, "usb task", 1024, NULL, TASK_PRIORITY_HIGHEST, NULL);
}
