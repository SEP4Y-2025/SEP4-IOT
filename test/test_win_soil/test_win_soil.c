#include "unity.h"
#include "soil.h"
#include "mock_avr_io.h"

void setUp(void)
{
    ADMUX = 0;
    ADCSRA = 0;
    ADCSRB = 0;
    DIDR2 = 0;
    ADC = 0;
}
void tearDown(void) {}

void test_soil_init_should_program_ADC_registers(void)
{
    soil_init();

    // ADMUX must only have REFS0 (AVcc) set
    TEST_ASSERT_EQUAL_HEX8((1 << REFS0), ADMUX);

    // ADCSRA must have ADEN + ADPS2:0 (prescaler 128)
    TEST_ASSERT_EQUAL_HEX8(
        (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0),
        ADCSRA);

    // DIDR2 must disable digital input on ADC8
    TEST_ASSERT_EQUAL_HEX8((1 << ADC8D), DIDR2);
}

void test_soil_read_should_select_channel_and_return_ADC_value(void)
{
    ADC = 0x2AB;

    uint16_t result = soil_read();

    const uint8_t mux_mask = ((1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3) | (1 << MUX4));
    TEST_ASSERT_EQUAL_HEX8(
        0,
        ADMUX & mux_mask);

    TEST_ASSERT_TRUE_MESSAGE(
        (ADCSRB & (1 << MUX5)) != 0,
        "Expected MUX5 bit set in ADCSRB");

    TEST_ASSERT_EQUAL_UINT16(0x2AB, result);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_soil_init_should_program_ADC_registers);
    RUN_TEST(test_soil_read_should_select_channel_and_return_ADC_value);
    return UNITY_END();
}
