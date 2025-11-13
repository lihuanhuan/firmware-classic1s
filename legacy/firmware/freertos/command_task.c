#include "command_task.h"
#include "task_header.h"
#include "user_messages.h"

#include "config.h"
#include "usb.h"
#include "ble.h"

extern void fido_task(void *pvParameters);
extern void ble_fido_task(void *pvParameters);
extern void ble_fido_poll(void);
extern void init_fido_timers(void);

static bool usb_lock = false;

static void usb_task(void *pvParameters) {
  (void)pvParameters;  
  
  if (!usb_lock) {
    usbInit();
  }

  while (1) {
    if (!usb_lock) {
      usb_poll();
    } else {
      ble_fido_poll();
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void create_command_task(void) {
  init_fido_timers();
  config_getUsblock(&usb_lock, true);
  change_ble_sta(usb_lock);
  xTaskCreate(usb_task, "usb task", 1024, NULL, TASK_PRIORITY_HIGH, NULL);
  if (!usb_lock) {
    xTaskCreate(fido_task, "fido task", 3072, NULL, TASK_PRIORITY_MEDIUM, NULL);
  } else {
    xTaskCreate(ble_fido_task, "ble fido task", 3072, NULL, TASK_PRIORITY_MEDIUM,
                NULL);
  }
}
