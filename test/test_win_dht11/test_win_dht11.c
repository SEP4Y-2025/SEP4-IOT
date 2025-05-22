// tests/test_win_sensors/test_win_dht11.c

#include "unity.h"
#include "../fff.h" // your fff library
#include "dht11.h"

//------------------------------------------------------------------------------
// Stub out the AVR pins/ports
//------------------------------------------------------------------------------
uint8_t DDRL;
uint8_t PORTL;
uint8_t PINL;

// The driver uses these macros:
#define DATA_BIT 1
#define DATA_DDR DDRL
#define DATA_PORT PORTL
#define DATA_PIN PINL

//------------------------------------------------------------------------------
// Stub delays — no real waiting
//------------------------------------------------------------------------------
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(_delay_ms, int);
FAKE_VOID_FUNC(_delay_us, int);

//------------------------------------------------------------------------------
// Test harness
//------------------------------------------------------------------------------
void setUp(void)
{
    // clear registers and reset fakes
    DDRL = PORTL = PINL = 0x00;
    RESET_FAKE(_delay_ms);
    RESET_FAKE(_delay_us);
}
void tearDown(void) {}

//------------------------------------------------------------------------------
// 1) dht11_init() is currently empty, so it must not touch DDRL or PORTL
//------------------------------------------------------------------------------
void test_dht11_init_should_do_nothing_to_pins(void)
{
    DDRL = 0xAA;
    PORTL = 0x55;

    dht11_init();

    TEST_ASSERT_EQUAL_HEX8(0xAA, DDRL);
    TEST_ASSERT_EQUAL_HEX8(0x55, PORTL);
}

void test_dht11_get_should_fail_and_zero_outputs_when_no_signal(void)
{
    // simulate line stuck high
    PINL = (1 << DATA_BIT);

    uint8_t hi = 0xFF;
    uint8_t hd = 0xFF;
    uint8_t ti = 0xFF;
    uint8_t td = 0xFF;

    DHT11_ERROR_MESSAGE_t err = dht11_get(&hi, &hd, &ti, &td);

    TEST_ASSERT_EQUAL_INT(DHT11_FAIL, err);
    TEST_ASSERT_EQUAL_UINT8(0, hi);
    TEST_ASSERT_EQUAL_UINT8(0, hd);
    TEST_ASSERT_EQUAL_UINT8(0, ti);
    TEST_ASSERT_EQUAL_UINT8(0, td);
}

void test_dht11_get_should_not_deref_null_pointers(void)
{
    // still no pulses → failure
    PINL = (1 << DATA_BIT);

    uint8_t hi = 0xFF, ti = 0xFF;
    // pass NULL for the decimal parts
    DHT11_ERROR_MESSAGE_t err = dht11_get(&hi, NULL, &ti, NULL);

    TEST_ASSERT_EQUAL_INT(DHT11_FAIL, err);
    TEST_ASSERT_EQUAL_UINT8(0, hi);
    TEST_ASSERT_EQUAL_UINT8(0, ti);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dht11_init_should_do_nothing_to_pins);
    RUN_TEST(test_dht11_get_should_fail_and_zero_outputs_when_no_signal);
    RUN_TEST(test_dht11_get_should_not_deref_null_pointers);
    return UNITY_END();
}
