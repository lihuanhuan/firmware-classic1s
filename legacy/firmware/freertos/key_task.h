#ifndef KEY_TASK_H
#define KEY_TASK_H

#include <stdint.h>

typedef struct {
  uint8_t value;
  uint8_t long_press;
} key_msg_t;

uint8_t key_wait_for_exit(uint32_t timeout);
void create_key_task(void);

#endif
