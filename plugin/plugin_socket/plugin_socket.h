#pragma once

typedef enum {
    REQUESTER = 0,
    RESPONDER,
    SOCKET_ROLE_LAST,
} socket_role_t;

int plugin_socket_init(const socket_role_t role, const char *addr);

int plugin_socket_destroy(void);

int plugin_socket_send_message(const char *message, const int len);

void *plugin_socket_get(void);
