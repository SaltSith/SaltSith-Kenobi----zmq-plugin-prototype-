#include "plugin.h"

#include <czmq.h>
#include <zmq.h>

#include <stdio.h>

#include <string.h>
#include <unistd.h>

typedef struct {
    void *socket_context;
    void *socket;
    zpoller_t *socket_poller;
} plugin_ctx_type_t;

typedef enum {
    REQUESTER = 0,
    RESPONDER,
    SOCKET_TYPE_LAST,
} socket_type_t;


static plugin_ctx_type_t plugin_ctx[SOCKET_TYPE_LAST];

int plugin_sockets_init(const char *requester_addr, const char *responder_addr)
{
    for (int plugin_socket = 0; plugin_socket < SOCKET_TYPE_LAST; plugin_socket++) {

        plugin_ctx[plugin_socket].socket_context = zmq_ctx_new();
        if (plugin_ctx[plugin_socket].socket_context == NULL) {
            return -1;
        }

        plugin_ctx[plugin_socket].socket = zmq_socket(plugin_ctx[plugin_socket].socket_context, plugin_socket == REQUESTER ? ZMQ_REQ : ZMQ_REP);
        if (plugin_ctx[plugin_socket].socket == NULL) {
            return -2;
        }

        if (plugin_socket == REQUESTER) {
            zmq_connect(plugin_ctx[REQUESTER].socket, requester_addr);
        } else {
            zmq_bind(plugin_ctx[RESPONDER].socket, responder_addr);
        }

        plugin_ctx[plugin_socket].socket_poller = zpoller_new(plugin_ctx[plugin_socket].socket, NULL);

        if (plugin_ctx[plugin_socket].socket_poller == NULL) {
            return -3;
        }
    }

    return 0;
}

void plugin_sockets_destroy(void)
{
    for (int plugin_socket = 0; plugin_socket < SOCKET_TYPE_LAST; plugin_socket++) {
        zmq_close(plugin_ctx[plugin_socket].socket);
        zmq_ctx_destroy(plugin_ctx[plugin_socket].socket_context);
        zpoller_destroy((zpoller_t **)&plugin_ctx[plugin_socket].socket_poller);
    }
}

void plugin_socket_loop(void)
{
    char buffer [100];

    zmq_send (plugin_ctx[REQUESTER].socket, "Kenobi Req", strlen("Kenobi Req"), 0);
    if (((zsock_t *)zpoller_wait(plugin_ctx[REQUESTER].socket_poller, 50)) != NULL) {
        memset(buffer, '\0', sizeof(buffer));
        zmq_recv (plugin_ctx[REQUESTER].socket, buffer, 100, 0);
        printf ("Received ack message %10s \r\n", buffer);
    }

    if (((zsock_t *)zpoller_wait(plugin_ctx[RESPONDER].socket_poller, 50)) != NULL) {
        memset(buffer, '\0', sizeof(buffer));
        zmq_recv (plugin_ctx[RESPONDER].socket, buffer, 100, 0);
        printf ("Received request: %10s\r\n", buffer);
        zmq_send (plugin_ctx[RESPONDER].socket, "ACK", strlen("ACK"), 0);
    }
}
