#pragma once

#include "zmq_plugin/zmq_plugin_socket/zmq_plugin_socket.h"

#define zmq_plugin_task_QUEUE_NAME  "/zmq_plugin_kenobi_queue"

#define PLUGIN_ADDR "tcp://*:5558"
#define PLUGIN_ROLE RESPONDER

#define wakeup_message "Arised\r\n"

#define kenobi_task_QUEUE_NAME "kenobi_queue"
