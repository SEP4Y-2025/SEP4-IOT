// mock_dht11.c
#include "mock_dht11.h"

DHT11_ERROR_MESSAGE_t mock_dht11_result = DHT11_OK;
uint8_t mock_humidity_int = 55;
uint8_t mock_humidity_dec = 0;
uint8_t mock_temp_int = 23;
uint8_t mock_temp_dec = 5;

void dht11_init(void) {
    // No-op
}

DHT11_ERROR_MESSAGE_t dht11_get(uint8_t* humidity_integer, uint8_t* humidity_decimal, uint8_t* temperature_integer, uint8_t* temperature_decimal) {
    if (humidity_integer) *humidity_integer = mock_humidity_int;
    if (humidity_decimal) *humidity_decimal = mock_humidity_dec;
    if (temperature_integer) *temperature_integer = mock_temp_int;
    if (temperature_decimal) *temperature_decimal = mock_temp_dec;
    return mock_dht11_result;
}
