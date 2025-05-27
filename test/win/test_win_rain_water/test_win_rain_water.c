#include "unity.h"
#include "rain_water_level_sensor.h"
#include "../fff.h"
DEFINE_FFF_GLOBALS;

void setUp(void)
{
    FFF_RESET_HISTORY();
}

void tearDown(void) {}
void test_read_raw_should_return_value_from_analogRead(void)
{
    analogRead_fake.return_val = 678;
    uint16_t val = rain_water_level_sensor_read_raw();
    TEST_ASSERT_EQUAL_UINT16(678, val);
    TEST_ASSERT_EQUAL_UINT8(A1, analogRead_fake.arg0_val);
}

void test_read_cm_should_convert_raw_to_cm(void)
{
    analogRead_fake.return_val = 1023;
    float cm = rain_water_level_sensor_read_cm();
    TEST_ASSERT_FLOAT_WITHIN(0.1, 10.0f, cm);

    analogRead_fake.return_val = 0;
    cm = rain_water_level_sensor_read_cm();
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0f, cm);

    analogRead_fake.return_val = 512;
    cm = rain_water_level_sensor_read_cm();
    TEST_ASSERT_FLOAT_WITHIN(0.1, 5.0f, cm);
}

void test_read_percentage_should_scale_and_clamp(void)
{
    analogRead_fake.return_val = 0;
    TEST_ASSERT_EQUAL_UINT8(0, rain_water_level_sensor_read_percentage());

    analogRead_fake.return_val = 614;  // ~60%
    TEST_ASSERT_EQUAL_UINT8(100, rain_water_level_sensor_read_percentage());

    analogRead_fake.return_val = 1023;
    TEST_ASSERT_EQUAL_UINT8(100, rain_water_level_sensor_read_percentage());

    analogRead_fake.return_val = 307;  // ~30% → ~50%
    uint8_t p = rain_water_level_sensor_read_percentage();
    TEST_ASSERT_UINT8_WITHIN(2, 50, p);  // Accept ~48–52
}
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_read_raw_should_return_value_from_analogRead);
    RUN_TEST(test_read_cm_should_convert_raw_to_cm);
    RUN_TEST(test_read_percentage_should_scale_and_clamp);
    return UNITY_END();
}
