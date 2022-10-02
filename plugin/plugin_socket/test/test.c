#include "../../../acutest.h"

#include "../plugin_socket.h"

#include "../../../config.h"

#include "string.h"

void test_plugin_socket_init_WRONG_ROLE(void) {
    TEST_ASSERT(plugin_socket_init(SOCKET_ROLE_LAST, PLUGIN_ADDR) == -1);
}

void test_plugin_socket_init_ADDR_NULL_PTR(void) {
    TEST_ASSERT(plugin_socket_init(PLUGIN_ROLE, NULL) == -1);
}

void test_plugin_socket_init(void) {
    TEST_ASSERT(plugin_socket_init(PLUGIN_ROLE, PLUGIN_ADDR) == 0);
}

void test_plugin_socket_destroy(void) {
    int result = plugin_socket_init(PLUGIN_ROLE, PLUGIN_ADDR);
    TEST_ASSERT(result == 0);

    result = plugin_socket_destroy();
    TEST_ASSERT(result == 0);
}

void test_plugin_socket_send_message_NULL_MESSAGE_PTR(void) {
    int result = plugin_socket_init(PLUGIN_ROLE, PLUGIN_ADDR);
    TEST_ASSERT(result == 0);

    result = plugin_socket_send_message(NULL, 20);
    TEST_ASSERT(result == -1);

    result = plugin_socket_destroy();
    TEST_ASSERT(result == 0);
}

void test_plugin_socket_send_message(void) {
    int result = plugin_socket_init(PLUGIN_ROLE, PLUGIN_ADDR);
    TEST_ASSERT(result == 0);

    result = plugin_socket_send_message("Hello\0", strlen("Hello\0"));
    TEST_ASSERT(result == -1);

    result = plugin_socket_destroy();
    TEST_ASSERT(result == 0);
}


TEST_LIST = {
	{"test_plugin_socket_init_WRONG_ROLE", test_plugin_socket_init_WRONG_ROLE},
	{"test_plugin_socket_init_ADDR_NULL_PTR", test_plugin_socket_init_ADDR_NULL_PTR},
	{"test_plugin_socket_init", test_plugin_socket_init},
	{"test_plugin_socket_destroy", test_plugin_socket_destroy},
	{"test_plugin_socket_send_message", test_plugin_socket_send_message},
	{0} /* must be terminated with 0 */
};