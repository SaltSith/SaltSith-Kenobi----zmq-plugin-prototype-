#include "plugin/plugin_task.h"


int main (void)
{
    
    if (plugin_task_init()) {
        return -1;
    }

    pthread_join(plugin_task_handle_get(), NULL);
    
    return 0;
}
