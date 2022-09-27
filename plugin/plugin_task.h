#pragma once

#include <pthread.h>

int plugin_task_init(void);

pthread_t plugin_task_handle_get(void);

void plugin_task_cleanup_handler(void *arg);

void plugin_task_send_msg(void *msg);
