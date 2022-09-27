#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <czmq.h>

<<<<<<< HEAD

static zpoller_t *poller_req;
static zpoller_t *poller_res;

int main (void)
{
    void *requester_context = zmq_ctx_new ();
    void *requester = zmq_socket (requester_context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");
    poller_req = zpoller_new(requester, NULL);

    void *responder_context = zmq_ctx_new ();
    void *responder = zmq_socket (responder_context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5556");
    poller_res = zpoller_new(responder, NULL);



    while (1) {
        char buffer [100];
        memset(buffer, '\0', sizeof(buffer));

        zmq_send (requester, "Kenobi Req", strlen("Kenobi Req"), 0);

        if (((zsock_t *)zpoller_wait(poller_req, 50)) != NULL) {
            zmq_recv (requester, buffer, 10, 0);
            printf ("Received %10s \r\n", buffer);
        }

        memset(buffer, '\0', sizeof(buffer));

        if (((zsock_t *)zpoller_wait(poller_res, 50)) != NULL) {
            memset(buffer, '\0', sizeof(buffer));
            zmq_recv (responder, buffer, 100, 0);
            printf ("Received %10s\r\n", buffer);
            zmq_send (responder, "ACK", strlen("ACK"), 0);
        }
    }

    zmq_close (requester);
    zmq_ctx_destroy (requester_context);

    zmq_close (responder);
    zmq_ctx_destroy (responder_context);

=======
#include "plugin/plugin_task.h"


int main (void)
{
    
    if (plugin_task_init()) {
        return -1;
    }

    pthread_join(plugin_task_handle_get(), NULL);
    
>>>>>>> develop
    return 0;
}
