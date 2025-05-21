#include <unity.h>

// Optional fixtures — you can omit both if you don't need setup/teardown.
void setUp(void) { /* runs before each test */ }
void tearDown(void) { /* runs after  each test */ }

// A single dummy test that always passes.
void test_dummy_always_passes(void)
{
    TEST_ASSERT_TRUE(1);
}

// You can omit main() entirely and let PlatformIO generate one.
// If you prefer to write it yourself, uncomment below:

/*
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dummy_always_passes);
    return UNITY_END();
}
*/
