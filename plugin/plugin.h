#pragma once

typedef enum {
    REQUESTER = 0,
    RESPONDER,
    SOCKET_TYPE_LAST,
} socket_type_t;

int plugin_socket_init(const socket_type_t plugin_socket, const char *addr);

void plugin_sockets_destroy(void);

int plugin_socket_reinit(const socket_type_t plugin_socket, const char *addr);

void plugin_socket_loop(void);
