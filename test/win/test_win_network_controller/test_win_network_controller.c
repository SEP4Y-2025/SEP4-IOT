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
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_network_controller_init_should_call_wifi_init);
    RUN_TEST(test_connect_ap_fails_if_no_known_ssid_found);
    RUN_TEST(test_connect_ap_succeeds_when_known_ssid_found);
    return UNITY_END();
}
