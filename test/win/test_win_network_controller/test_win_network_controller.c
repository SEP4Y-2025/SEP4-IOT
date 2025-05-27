// tests/test_win_network_controller/test_win_network_controller.c
#include "unity.h"
#include "../fff.h"
#include "wifi.h"

// Pull in the implementation (which pulls in wifi.h)
#include "../../src/controllers/network_controller.c"
// 1) Turn off LOG so it doesn’t call into logger_service
#ifdef LOG
#undef LOG
#endif
#define LOG(...)

void logger_service_log_internal(const char *file,
                                 int line,
                                 const char *fmt,
                                 ...)
{
    (void)file;
    (void)line;
    (void)fmt;
}


DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(wifi_init);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_disable_echo);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_set_mode_to_1);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_set_to_single_Connection);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_join_AP, char*, char*);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_quit_AP);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_CWJAP_status);
typedef void (*callback_t)(void);

FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_create_TCP_connection, char*, uint16_t, callback_t, char*);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_TCP_transmit, uint8_t*, uint16_t);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_close_TCP_connection);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_scan_APs, uint16_t);
FAKE_VALUE_FUNC(const char*, wifi_get_scan_buffer);
FAKE_VALUE_FUNC(uint16_t, wifi_get_scan_buffer_len);
void setUp(void) {
    RESET_FAKE(wifi_init);
    RESET_FAKE(wifi_command_disable_echo);
    RESET_FAKE(wifi_command_set_mode_to_1);
    RESET_FAKE(wifi_command_set_to_single_Connection);
    RESET_FAKE(wifi_command_join_AP);
    RESET_FAKE(wifi_command_quit_AP);
    RESET_FAKE(wifi_command_CWJAP_status);
    RESET_FAKE(wifi_command_create_TCP_connection);
    RESET_FAKE(wifi_command_TCP_transmit);
    RESET_FAKE(wifi_command_close_TCP_connection);
    RESET_FAKE(wifi_scan_APs);
    RESET_FAKE(wifi_get_scan_buffer);
    RESET_FAKE(wifi_get_scan_buffer_len);
}

void tearDown(void) {}
void test_network_controller_init_should_call_wifi_init(void) {
    network_controller_init(NULL, NULL);
    TEST_ASSERT_EQUAL(1, wifi_init_fake.call_count);
}
void test_connect_ap_fails_if_no_known_ssid_found(void) {
    // Simulate scan returning some SSIDs not in known list
    wifi_scan_APs_fake.return_val = WIFI_OK;
    wifi_get_scan_buffer_fake.return_val = "+CWLAP:(3,\"WrongSSID\",-50)\r\n";
    wifi_get_scan_buffer_len_fake.return_val = strlen("+CWLAP:(3,\"WrongSSID\",-50)\r\n");

    bool result = (network_controller_connect_ap(3) == WIFI_FAIL);
    TEST_ASSERT_TRUE(result);
}
void test_connect_ap_succeeds_when_known_ssid_found(void) {
    // Simulate known AP in list
    wifi_scan_APs_fake.return_val = WIFI_OK;
    wifi_get_scan_buffer_fake.return_val =
        "+CWLAP:(3,\"Test_SSID\",-42)\r\n";
    wifi_get_scan_buffer_len_fake.return_val =
        strlen("+CWLAP:(3,\"Test_SSID\",-42)\r\n");

    wifi_command_join_AP_fake.return_val = WIFI_OK;


    WIFI_ERROR_MESSAGE_t result = network_controller_connect_ap(5);
    TEST_ASSERT_EQUAL(WIFI_OK, result);
}
void test_network_controller_setup_should_return_true_when_all_ok(void) {
    wifi_command_disable_echo_fake.return_val = WIFI_OK;
    wifi_command_set_mode_to_1_fake.return_val = WIFI_OK;
    wifi_command_set_to_single_Connection_fake.return_val = WIFI_OK;

    TEST_ASSERT_TRUE(network_controller_setup());
}

void test_network_controller_setup_should_return_false_on_failure(void) {
    wifi_command_disable_echo_fake.return_val = WIFI_FAIL;

    TEST_ASSERT_FALSE(network_controller_setup());
}

void test_network_controller_disconnect_ap_should_return_ok(void) {
    wifi_command_quit_AP_fake.return_val = WIFI_OK;

    TEST_ASSERT_EQUAL(WIFI_OK, network_controller_disconnect_ap());
}

void test_network_controller_disconnect_ap_should_return_fail(void) {
    wifi_command_quit_AP_fake.return_val = WIFI_FAIL;

    TEST_ASSERT_EQUAL(WIFI_FAIL, network_controller_disconnect_ap());
}

void test_network_controller_is_tcp_connected(void) {
    // Should reflect _tcp_connected internal flag
    // By opening a TCP connection
    wifi_command_create_TCP_connection_fake.return_val = WIFI_OK;

    network_controller_tcp_open("192.168.0.1", 1234);
    TEST_ASSERT_TRUE(network_controller_is_tcp_connected());
}

void test_network_controller_is_ap_connected_should_return_true_if_status_ok_and_cwjap_in_buffer(void) {
    wifi_command_CWJAP_status_fake.return_val = WIFI_OK;
    wifi_get_scan_buffer_fake.return_val = "+CWJAP:\"SSID\"";

    TEST_ASSERT_TRUE(network_controller_is_ap_connected());
}

void test_network_controller_is_ap_connected_should_return_false_on_status_fail(void) {
    wifi_command_CWJAP_status_fake.return_val = WIFI_FAIL;

    TEST_ASSERT_FALSE(network_controller_is_ap_connected());
}

void test_network_controller_is_ap_connected_should_return_false_if_cwjap_missing(void) {
    wifi_command_CWJAP_status_fake.return_val = WIFI_OK;
    wifi_get_scan_buffer_fake.return_val = "NO_AP";

    TEST_ASSERT_FALSE(network_controller_is_ap_connected());
}

void test_network_controller_tcp_open_should_return_ok_and_set_connected_true(void) {
    wifi_command_create_TCP_connection_fake.return_val = WIFI_OK;

    WIFI_ERROR_MESSAGE_t r = network_controller_tcp_open("1.2.3.4", 1883);
    TEST_ASSERT_EQUAL(WIFI_OK, r);
    TEST_ASSERT_TRUE(network_controller_is_tcp_connected());
}

void test_network_controller_tcp_open_should_return_fail_and_set_connected_false(void) {
    wifi_command_create_TCP_connection_fake.return_val = WIFI_FAIL;

    WIFI_ERROR_MESSAGE_t r = network_controller_tcp_open("1.2.3.4", 1883);
    TEST_ASSERT_EQUAL(WIFI_FAIL, r);
    TEST_ASSERT_FALSE(network_controller_is_tcp_connected());
}

void test_network_controller_tcp_send_should_return_true_on_success(void) {
    wifi_command_TCP_transmit_fake.return_val = WIFI_OK;

    const uint8_t data[] = "hello";
    TEST_ASSERT_TRUE(network_controller_tcp_send(data, sizeof(data)));
}

void test_network_controller_tcp_send_should_return_false_on_failure(void) {
    wifi_command_TCP_transmit_fake.return_val = WIFI_FAIL;

    const uint8_t data[] = "hello";
    TEST_ASSERT_FALSE(network_controller_tcp_send(data, sizeof(data)));
}

void test_network_controller_tcp_close_should_return_true_on_success(void) {
    wifi_command_close_TCP_connection_fake.return_val = WIFI_OK;

    TEST_ASSERT_TRUE(network_controller_tcp_close());
}

void test_network_controller_tcp_close_should_return_false_on_failure(void) {
    wifi_command_close_TCP_connection_fake.return_val = WIFI_FAIL;

    TEST_ASSERT_FALSE(network_controller_tcp_close());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_network_controller_init_should_call_wifi_init);
    RUN_TEST(test_connect_ap_fails_if_no_known_ssid_found);
    RUN_TEST(test_connect_ap_succeeds_when_known_ssid_found);
    RUN_TEST(test_network_controller_setup_should_return_true_when_all_ok);
    RUN_TEST(test_network_controller_setup_should_return_false_on_failure);
    RUN_TEST(test_network_controller_disconnect_ap_should_return_ok);
    RUN_TEST(test_network_controller_disconnect_ap_should_return_fail);
    RUN_TEST(test_network_controller_is_tcp_connected);
    RUN_TEST(test_network_controller_is_ap_connected_should_return_true_if_status_ok_and_cwjap_in_buffer);
    RUN_TEST(test_network_controller_is_ap_connected_should_return_false_on_status_fail);
    RUN_TEST(test_network_controller_is_ap_connected_should_return_false_if_cwjap_missing);
    RUN_TEST(test_network_controller_tcp_open_should_return_ok_and_set_connected_true);
    RUN_TEST(test_network_controller_tcp_open_should_return_fail_and_set_connected_false);
    RUN_TEST(test_network_controller_tcp_send_should_return_true_on_success);
    RUN_TEST(test_network_controller_tcp_send_should_return_false_on_failure);
    RUN_TEST(test_network_controller_tcp_close_should_return_true_on_success);
    RUN_TEST(test_network_controller_tcp_close_should_return_false_on_failure);

    return UNITY_END();
}
