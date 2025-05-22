#ifndef MOCK_DHT11_H
#define MOCK_DHT11_H

#include <stdint.h>

typedef enum {
    DHT11_OK,
    DHT11_FAIL
} DHT11_ERROR_MESSAGE_t;

void dht11_init(void);

// Mock return values
extern DHT11_ERROR_MESSAGE_t mock_dht11_result;
extern uint8_t mock_humidity_int;
extern uint8_t mock_humidity_dec;
extern uint8_t mock_temp_int;
extern uint8_t mock_temp_dec;

DHT11_ERROR_MESSAGE_t dht11_get(uint8_t* humidity_integer, uint8_t* humidity_decimal, uint8_t* temperature_integer, uint8_t* temperature_decimal);

#endif
