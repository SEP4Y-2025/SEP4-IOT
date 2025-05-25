#include "unity.h"
#include "uart.h"
#include "../fff.h"

DEFINE_FFF_GLOBALS
FAKE_VOID_FUNC(sei);
FAKE_VOID_FUNC(cli);
FAKE_VOID_FUNC(uart_init, USART_t, uint32_t, UART_Callback_t);

void setUp(void)
{
  RESET_FAKE(sei);
  RESET_FAKE(cli);
  RESET_FAKE(uart_init);
}

void tearDown(void)
{
  // clean stuff up here
}

void test_uart_init0()
{
  uart_init(USART_0, 9600, NULL);
  TEST_ASSERT_EQUAL_INT(1, uart_init_fake.call_count);
  TEST_ASSERT_EQUAL_PTR(NULL, uart_init_fake.arg2_val);
}

void test_uart_init1()
{
  uart_init(USART_1, 9600, NULL);
  TEST_ASSERT_EQUAL_INT(1, uart_init_fake.call_count);
}

void test_uart_init3()
{
  uart_init(USART_3, 9600, NULL);
  TEST_ASSERT_EQUAL_INT(1, uart_init_fake.call_count);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_uart_init0);
  RUN_TEST(test_uart_init1);
  RUN_TEST(test_uart_init3);
  return UNITY_END();
}