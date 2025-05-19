#include "command_task.h"
#include "task_header.h"
#include "user_messages.h"

#include "usb.h"

extern void fido_task(void *pvParameters);
extern void init_fido_timers(void);

static void usb_task(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    usb_poll();
    vTaskDelay(pdMS_TO_TICKS(2));
  }
}

void create_command_task(void) {
  init_fido_timers();
  xTaskCreate(usb_task, "usb task", 1024, NULL, TASK_PRIORITY_HIGH, NULL);
  xTaskCreate(fido_task, "fido task", 3072, NULL, TASK_PRIORITY_MEDIUM, NULL);
}
