#include <unity.h>
#include "fff.h"
#include "services/logger_service.h"
#include "controllers/pc_comm_controller.h"

DEFINE_FFF_GLOBALS;

// Setup fake functions
FAKE_VOID_FUNC2(pc_comm_controller_init, uint32_t, pc_comm_callback_t);
FAKE_VOID_FUNC1(pc_comm_controller_send_str, const char *);

void setUp(void) {
    RESET_FAKE(pc_comm_controller_init);
    RESET_FAKE(pc_comm_controller_send_str);
    FFF_RESET_HISTORY();
}

void tearDown(void) {
    // This function is called after each test
}


void test_logger_service_init_calls_pc_comm_init(void) {
    logger_service_init(115200);
    TEST_ASSERT_EQUAL(1, pc_comm_controller_init_fake.call_count);
    TEST_ASSERT_EQUAL(9600, pc_comm_controller_init_fake.arg0_val);
    TEST_ASSERT_NULL(pc_comm_controller_init_fake.arg1_val);
}

void test_logger_service_log_internal_sends_formatted_string(void) {
    const char *test_msg = "This is a test";
    logger_service_log_internal("file.c", 42, test_msg);

    TEST_ASSERT_EQUAL(1, pc_comm_controller_send_str_fake.call_count);

    const char *sent_str = pc_comm_controller_send_str_fake.arg0_val;
    TEST_ASSERT_NOT_NULL(sent_str);

    TEST_ASSERT_TRUE(strstr(sent_str, "file.c") != NULL);
    TEST_ASSERT_TRUE(strstr(sent_str, "42") != NULL);
    TEST_ASSERT_TRUE(strstr(sent_str, test_msg) != NULL);
}

