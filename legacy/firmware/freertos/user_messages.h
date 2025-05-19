#ifndef USER_MESSAGES_H
#define USER_MESSAGES_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

extern QueueHandle_t ui_lcd_msg_queue;
extern QueueHandle_t key_msg_queue;
extern SemaphoreHandle_t system_state_semaphore;

void user_messages_init(void);

#endif
