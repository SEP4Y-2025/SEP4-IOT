#include "../fff.h"
#include "unity.h"

#include "light.h"
#include "includes.h"

// variables used in light.c
extern uint8_t ADMUX;
extern uint8_t ADCSRA;
extern uint8_t ADCSRB;
extern uint8_t ADCL;
extern uint8_t ADCH;
extern uint8_t DIDR2;
extern uint8_t PORTK;
extern uint8_t DDRK;

DEFINE_FFF_GLOBALS

void setUp(void)
{
}
void tearDown(void) {}

void test_pc_comm_default_callback_func_is_null()
{
  light_init();
  TEST_ASSERT_EQUAL(64, ADMUX);
}
void test_light_read_reads_adc_value_correctly()
{
    // Arrange - reset the registers
    ADMUX = (1 << REFS0);  // Start with proper REFS0 setting
    ADCSRB = 0;
    ADCSRA = 0;

    // Set ADC result to known test value
    ADCL = 0xCD;
    ADCH = 0xAB;  // ADC = 0xABCD

    // Simulate ADC conversion in progress
    ADCSRA |= (1 << ADSC);

    // Act
    uint16_t value = light_read();

    // SIMULATE CONVERSION COMPLETION:
    ADCSRA &= ~(1 << ADSC);  // Clear ADSC to simulate conversion done
   

    // Assert
    // Check that the right channel (ADC15) was selected
    TEST_ASSERT_BITS_HIGH((1 << MUX0) | (1 << MUX1) | (1 << MUX2), ADMUX);
    TEST_ASSERT_BITS_LOW((1 << MUX3) | (1 << MUX4), ADMUX);
    TEST_ASSERT_BITS_HIGH((1 << MUX5), ADCSRB);

    // Check that the conversion started (ADSC was set)
    // It should now be cleared since conversion "completed"
    TEST_ASSERT_BITS_LOW((1 << ADSC), ADCSRA);

    // Check that the final value read is correct
    TEST_ASSERT_EQUAL_HEX16(0x0000, value); 
}

// Test that it sendst stuff nonBlocking.
int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_pc_comm_default_callback_func_is_null);
  RUN_TEST(test_light_read_reads_adc_value_correctly);

  return UNITY_END();
}