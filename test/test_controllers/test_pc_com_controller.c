#include "unity.h"
#include "../fff.h"

typedef void (*pc_comm_callback_t)(char c);

// 1) Instantiate the fff globals.
DEFINE_FFF_GLOBALS;

// 2) Declare the fakes.
FAKE_VOID_FUNC(pc_comm_init, uint32_t, pc_comm_callback_t);
FAKE_VOID_FUNC(pc_comm_send_array_blocking, uint8_t *, uint16_t);
FAKE_VOID_FUNC(pc_comm_send_string_blocking, char *);

// 3) Now include the controller under test.
#include "controllers/pc_comm_controller.h"

void setUp(void)
{
    // Reset all fakes before each test
    RESET_FAKE(pc_comm_init);
    RESET_FAKE(pc_comm_send_array_blocking);
    RESET_FAKE(pc_comm_send_string_blocking);
}

void tearDown(void) {}

void test_pc_comm_controller_init_Calls_pc_comm_init(void)
{
    uint32_t baud = 115200;
    pc_comm_callback_t cb = (pc_comm_callback_t)0xDEADBEEF;

    pc_comm_controller_init(baud, cb);

    // ensure it was called exactly once
    TEST_ASSERT_EQUAL_INT(1, pc_comm_init_fake.call_count);
    TEST_ASSERT_EQUAL_UINT32(baud, pc_comm_init_fake.arg0_val);
    TEST_ASSERT_EQUAL_PTR(cb, pc_comm_init_fake.arg1_val);
}

void test_pc_comm_controller_send_Calls_send_array(void)
{
    uint8_t buffer[] = {0xAA, 0xBB, 0xCC};
    uint16_t len = sizeof(buffer);

    bool ok = pc_comm_controller_send(buffer, len);

    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_INT(1, pc_comm_send_array_blocking_fake.call_count);
    TEST_ASSERT_EQUAL_PTR(buffer, pc_comm_send_array_blocking_fake.arg0_val);
    TEST_ASSERT_EQUAL_UINT16(len, pc_comm_send_array_blocking_fake.arg1_val);
}

void test_pc_comm_controller_send_str_Calls_send_string(void)
{
    const char *msg = "Hello, UART!";

    bool ok = pc_comm_controller_send_str(msg);

    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_INT(1, pc_comm_send_string_blocking_fake.call_count);
    TEST_ASSERT_EQUAL_STRING(msg, pc_comm_send_string_blocking_fake.arg0_val);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_pc_comm_controller_init_Calls_pc_comm_init);
    RUN_TEST(test_pc_comm_controller_send_Calls_send_array);
    RUN_TEST(test_pc_comm_controller_send_str_Calls_send_string);

    return UNITY_END();
}
