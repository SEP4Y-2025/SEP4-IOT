#include "../fff.h"
#include "unity.h"

#include "light.h"

#include <stdio.h>
#include <stdint.h>

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

// Test that it sendst stuff nonBlocking.

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_pc_comm_default_callback_func_is_null);

  return UNITY_END();
}