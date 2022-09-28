#include "plugin_task.h"

#include "plugin.h"

#include "../config.h"

#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>


#define PLUGIN_TASK_QUEUE_NAME  "/plugin_kenobi2"

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
    int result;
    unsigned int msg_prio;

    plugin_task_event_t event;
    int msg_len = sizeof(plugin_task_event_t);

    result = mq_receive(plugin_task_queue,
                        (char *)&event,
                        msg_len,
                        &msg_prio);

    if (result > 0) {
        plugin_task_event_handlers[event.event](event.args);
    }
}

static void 
*plugin_task(void *arg)
{

    int result;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(plugin_task_queue, &rfds);

    pthread_cleanup_push(plugin_task_cleanup_handler, NULL);

    plugin_ready = true;

    while (1) {
        plugin_socket_loop();

        result = select(plugin_task_queue + 1, &rfds, NULL, NULL, NULL);

        if (result == -1) {
            printf("Error on select.\r\n");
        } else if (result) {
            printf("Data is available on plugin task queue.\r\n");
            plugin_task_pop_message();
        } else {
            printf("No data within given timeout.\r\n");
        }
    }
    
    pthread_cleanup_pop(NULL);
    
    return NULL;
}

static int
plugin_task_queue_init(void)
{
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(plugin_task_event_t);
    attr.mq_curmsgs = 0;

    plugin_task_queue = mq_open(PLUGIN_TASK_QUEUE_NAME,
                                O_CREAT | /*O_EXCL |*/ O_NONBLOCK | O_RDWR,
                                0777,//S_IRUSR | S_IWUSR,
                                &attr);

    return plugin_task_queue;
}

int
plugin_task_init(void)
{
    int result = -1;

    if (plugin_socket_init(REQUESTER, REQUESTER_ADDR)) {
        return -1;
    }

    if (plugin_socket_init(RESPONDER, RESPONDER_ADDR)) {
        return -2;
    }


    // result = plugin_task_queue_init();
    // printf("plugin_task_queue_init %d\r\n", errno);
    // if (result == -1) {
    //     return result;
    // }

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

void
plugin_task_send_msg(void *msg)
{
    if (msg == NULL) {
        return;
    }

    plugin_task_event_t event = {
        .args = msg,
        .event = PLUGIN_EVENT_SEND_MESSAGE
    };

    while(!plugin_ready);

    int result = mq_send(plugin_task_queue,
                         (char *)&event,
                         sizeof(plugin_task_event_t),
                         0);

    return result == 0 ? true : false;
}
