#include "../../acutest.h"

#include "../plugin_task.h"

#include "string.h"

void test_plugin_task_init(void) {
    TEST_ASSERT(plugin_task_init() == 0);
}

void test_plugin_task_task_handle_get(void) {
    int result = plugin_task_init();
    TEST_ASSERT(result == 0);

    pthread_t pthread_ptr = plugin_task_handle_get();
    TEST_ASSERT(pthread_ptr != (pthread_t)NULL);
}

void test_plugin_task_send_message_NULL_MESSAGE_PTR(void) {
    int result = plugin_task_init();
    TEST_ASSERT(result == 0);

    pthread_t pthread_ptr = plugin_task_handle_get();
    TEST_ASSERT(pthread_ptr != (pthread_t)NULL);

    TEST_ASSERT(plugin_task_send_msg(NULL) == false);
}

void test_plugin_task_send_message(void) {
    int result = plugin_task_init();
    TEST_ASSERT(result == 0);

    pthread_t pthread_ptr = plugin_task_handle_get();
    TEST_ASSERT(pthread_ptr != (pthread_t)NULL);

    TEST_ASSERT(plugin_task_send_msg("Hello\0") == true);
}


TEST_LIST = {
	{"test_plugin_task_init", test_plugin_task_init},
	{"test_plugin_task_task_handle_get", test_plugin_task_task_handle_get},
	{"test_plugin_task_send_message_NULL_MESSAGE_PTR", test_plugin_task_send_message_NULL_MESSAGE_PTR},
	{"test_plugin_task_send_message", test_plugin_task_send_message},
	{0} /* must be terminated with 0 */
};