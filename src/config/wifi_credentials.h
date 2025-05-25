// This file is Auto Generated in platformio.ini file from credentials.csv
#include "wifi.h"
#include <stdint.h>

#ifndef TEST_WIFI_CREDENTIALS_OVERRIDE
static const wifi_credential_t known_credentials[] = {
    {.ssid = "MartyPartyPhone", .password = "123456789"},
    {.ssid = "JanPhone", .password = "Hello World"},
    {.ssid = "04898804_2.4GHz", .password = "08400381"},
};
#else
static const wifi_credential_t known_credentials[] = {
    {.ssid = "Test_SSID", .password = "TestPass1"},
};
#endif

static const uint8_t known_credentials_len = sizeof(known_credentials) / sizeof(known_credentials[0]);

static const char *mqtt_broker_ip = "172.20.10.2";
static const int mqtt_broker_port = 1883;
