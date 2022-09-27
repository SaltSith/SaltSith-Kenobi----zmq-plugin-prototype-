#pragma once

int plugin_sockets_init(const char *requester_addr, const char *responder_addr);

void plugin_sockets_destroy(void);

void plugin_socket_loop(void);