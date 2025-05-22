// test/test_network_controller.c

#include <unity.h>
#include "fff.h"

#include "controllers/network_controller.h"

// Declare the FFF mocks for all underlying `wifi_command_*` calls:
DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_disable_echo);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_set_mode_to_1);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_set_to_single_Connection);

FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_create_TCP_connection,
                char *, uint16_t, WIFI_TCP_Callback_t, char *);

FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_close_TCP_connection);

void setUp(void)
{
    // Reset all mocks before each test
    FFF_RESET_HISTORY();
    wifi_command_disable_echo_fake.return_val = WIFI_OK;
    wifi_command_set_mode_to_1_fake.return_val = WIFI_OK;
    wifi_command_set_to_single_Connection_fake.return_val = WIFI_OK;
    wifi_command_create_TCP_connection_fake.return_val = WIFI_OK;
    wifi_command_close_TCP_connection_fake.return_val = WIFI_OK;
}

void tearDown(void)
{
    // Nothing to clean up
}

// Test: successful setup sequence
void test_network_controller_setup_success(void)
{
    bool ok = network_controller_setup();
    TEST_ASSERT_TRUE_MESSAGE(ok, "Expected setup to succeed when all commands return WIFI_OK");
    // verify order of calls:
    TEST_ASSERT_EQUAL_PTR(1, wifi_command_disable_echo_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(1, wifi_command_set_mode_to_1_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(1, wifi_command_set_to_single_Connection_fake.call_count);
}

// Test: failure if disable_echo fails
void test_network_controller_setup_echo_fail(void)
{
    wifi_command_disable_echo_fake.return_val = WIFI_FAIL;
    bool ok = network_controller_setup();
    TEST_ASSERT_FALSE_MESSAGE(ok, "Expected setup to fail when disable_echo fails");
    // subsequent commands shouldn’t be called:
    TEST_ASSERT_EQUAL_PTR(0, wifi_command_set_mode_to_1_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(0, wifi_command_set_to_single_Connection_fake.call_count);
}

// Test: TCP open sets the connected flag
void test_network_controller_tcp_open_success(void)
{
    bool cb_invoked = false;
    // dummy callback
    network_controller_init(NULL, NULL); // _tcp_cb/_tcp_buf set but we ignore them
    wifi_command_create_TCP_connection_fake.return_val = WIFI_OK;

    WIFI_ERROR_MESSAGE_t r = network_controller_tcp_open("1.2.3.4", 1234);
    TEST_ASSERT_EQUAL_INT(WIFI_OK, r);
    TEST_ASSERT_TRUE_MESSAGE(network_controller_is_tcp_connected(),
                             "Expected _tcp_connected == true on WIFI_OK");
}

// Test: TCP open failure leaves disconnected
void test_network_controller_tcp_open_fail(void)
{
    network_controller_init(NULL, NULL);
    wifi_command_create_TCP_connection_fake.return_val = WIFI_FAIL;

    WIFI_ERROR_MESSAGE_t r = network_controller_tcp_open("1.2.3.4", 1234);
    TEST_ASSERT_EQUAL_INT(WIFI_FAIL, r);
    TEST_ASSERT_FALSE_MESSAGE(network_controller_is_tcp_connected(),
                              "Expected _tcp_connected == false on WIFI_FAIL");
}

// Test: TCP close returns underlying result
void test_network_controller_tcp_close(void)
{
    wifi_command_close_TCP_connection_fake.return_val = WIFI_OK;
    TEST_ASSERT_TRUE(network_controller_tcp_close());

    wifi_command_close_TCP_connection_fake.return_val = WIFI_FAIL;
    TEST_ASSERT_FALSE(network_controller_tcp_close());
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_network_controller_setup_success);
    RUN_TEST(test_network_controller_setup_echo_fail);
    RUN_TEST(test_network_controller_tcp_open_success);
    RUN_TEST(test_network_controller_tcp_open_fail);
    RUN_TEST(test_network_controller_tcp_close);

    return UNITY_END();
}
