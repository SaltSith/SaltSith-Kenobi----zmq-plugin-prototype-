#include "plugin_task.h"

#include "plugin_socket.h"

#include "../config.h"

#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <zmq.h>
#include <czmq.h>


#define PLUGIN_TASK_QUEUE_NAME  "/plugin_kenobi2"

#define POLL_TIMEOUT_DEFAULT    (50)
#define POLL_TIMEOUT_INFINITY   (-1)
#define POLL_TIMEOUT            POLL_TIMEOUT_DEFAULT

typedef enum {
    PLUGIN_EVENT_SEND_MESSAGE = 0,
    PLUGIN_EVENT_LAST
} plugin_task_event_type_t;

typedef struct {
    plugin_task_event_type_t event;
    void *args;
} plugin_task_event_t;

static bool plugin_ready;
static pthread_t plugin_task_handle;
static int plugin_task_queue = -1;

static zmq_pollitem_t items[3];


typedef void(*plugin_task_event_handler)(void *args);

static void plugin_task_event_handler_send_msg(void *args);

plugin_task_event_handler plugin_task_event_handlers[PLUGIN_EVENT_LAST] = {
    plugin_task_event_handler_send_msg,
};

static void
plugin_task_event_handler_send_msg(void *args)
{

    printf("Sending message from handler\r\n");
}


static void *plugin_task(void *arg);

static int plugin_task_queue_init(void);

static void
plugin_task_pop_message(void)
{
    unsigned int msg_prio;

    plugin_task_event_t event;

    int msg_len = sizeof(plugin_task_event_t);

    int result = mq_receive(plugin_task_queue,
                            (char *)&event,
                            msg_len,
                            &msg_prio);

    if (result > 0) {
        plugin_task_event_handlers[event.event](event.args);
    }
}

static void plugin_task_check_queues(void)
{
    if (items[0].revents & ZMQ_POLLIN) {
        items[0].revents &= ~ZMQ_POLLIN;

        printf("Data is available on plugin task queue.\r\n");
        plugin_task_pop_message();
    }

    if (items[1].revents & ZMQ_POLLIN) {
        items[1].revents &= ~ZMQ_POLLIN;

        char buffer [100];
        memset(buffer, '\0', sizeof(buffer));
        zmq_recv(plugin_socket_get(REQUESTER), buffer, 100, 0);
        printf("Received ack message %10s \r\n", buffer);
        if (memcmp(buffer, wakeup_message, strlen(wakeup_message)) == 0) {
            printf("------ REINIT------------\r\n");
            plugin_socket_reinit(REQUESTER, REQUESTER_ADDR);
        }
    }

    if (items[2].revents & ZMQ_POLLIN) {
        items[2].revents &= ~ZMQ_POLLIN;

        char buffer[100];
        zmq_recv(plugin_socket_get(RESPONDER), buffer, 100, 0);
        printf("Received request: %10s\r\n", buffer);
        zmq_send(plugin_socket_get(RESPONDER), "ACK", strlen("ACK"), 0);
    }
}

static void 
*plugin_task(void *arg)
{
    fd_set rfds;

    FD_ZERO(&rfds);
    FD_SET(plugin_task_queue, &rfds);

    pthread_cleanup_push(plugin_task_cleanup_handler, NULL);

    plugin_ready = true;

    plugin_socket_send_message(wakeup_message, strlen(wakeup_message));

    items[0].socket = NULL;
    items[0].fd     = plugin_task_queue;
    items[0].events = ZMQ_POLLIN;

    items[1].socket = plugin_socket_get(REQUESTER);
    items[1].events = ZMQ_POLLIN;

    items[2].socket = plugin_socket_get(RESPONDER);
    items[2].events = ZMQ_POLLIN;

    while (1) {
        int rc = zmq_poll(items, sizeof(items) / sizeof(items[0]), POLL_TIMEOUT);

        if (rc > 0) {
            plugin_task_check_queues();
        }

        printf("-------------------Plugin task loop--------------\r\n");
    }
    
    pthread_cleanup_pop(NULL);
    
    return NULL;
}

static int
plugin_task_queue_init(void)
{
    struct mq_attr attr;

    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = sizeof(plugin_task_event_t);
    attr.mq_curmsgs = 0;

    plugin_task_queue = mq_open(PLUGIN_TASK_QUEUE_NAME,
                                O_CREAT | O_NONBLOCK | O_RDWR,
                                0777,
                                &attr);

    return plugin_task_queue;
}

int
plugin_task_init(void)
{

    if (plugin_socket_init(REQUESTER, REQUESTER_ADDR)) {
        return -1;
    }

    if (plugin_socket_init(RESPONDER, RESPONDER_ADDR)) {
        return -2;
    }

    int result = plugin_task_queue_init();

    if (result == -1) {
        return result;
    }

    result = pthread_create(&plugin_task_handle,
                            NULL,
                            plugin_task,
                            "plugin_task");

    return result;
}

pthread_t 
plugin_task_handle_get(void)
{

    return plugin_task_handle;
}

void 
plugin_task_cleanup_handler(void *arg)
{
    mq_unlink(PLUGIN_TASK_QUEUE_NAME);

    plugin_sockets_destroy();
}

bool
plugin_task_send_msg(void *msg)
{
    if (msg == NULL) {
        return false;
    }

    plugin_task_event_t event = {
        .args  = msg,
        .event = PLUGIN_EVENT_SEND_MESSAGE
    };

    while(!plugin_ready);

    int result = mq_send(plugin_task_queue,
                         (char *)&event,
                         sizeof(plugin_task_event_t),
                         0);

    return result == 0 ? true : false;
}
