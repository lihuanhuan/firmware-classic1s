#ifndef USER_MESSAGES_H
#define USER_MESSAGES_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

extern QueueHandle_t ui_msg_queue;
extern QueueHandle_t ui_key_msg_queue, cmd_key_msg_queue;
extern QueueHandle_t fido_msg_queue;
extern SemaphoreHandle_t system_state_semaphore;

void user_messages_init(void);

#endif
