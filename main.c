#include "zmq_plugin/zmq_plugin_task.h"


int main (void)
{
    
    if (zmq_plugin_task_init()) {
        return -1;
    }

    pthread_join(zmq_plugin_task_handle_get(), NULL);
    
    return 0;
}
