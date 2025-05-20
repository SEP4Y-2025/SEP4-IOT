// test/test_watering_service.c

#include <stdint.h>
#include "unity.h"
#include "fff.h"
#include "services/watering_service.h"

// === FFF Globals & Fakes ===
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC0(pump_controller_init);
FAKE_VOID_FUNC1(activate_pump, uint32_t);
FAKE_VALUE_FUNC0(int, rain_water_level_sensor_read_raw);

void setUp(void) {
    RESET_FAKE(pump_controller_init);
    RESET_FAKE(activate_pump);
    RESET_FAKE(rain_water_level_sensor_read_raw);
}

void tearDown(void) {}

// 1) watering_service_init() should not touch hardware:
void test_watering_service_init_does_nothing(void) {
    watering_service_init();
    TEST_ASSERT_EQUAL_UINT32(0, pump_controller_init_fake.call_count);
    TEST_ASSERT_EQUAL_UINT32(0, activate_pump_fake.call_count);
    TEST_ASSERT_EQUAL_UINT32(0, rain_water_level_sensor_read_raw_fake.call_count);
}

// 2) watering_service_water_pot() reads the sensor once and fires pump exactly once:
void test_watering_service_water_pot_activates_pump_correctly(void) {
    rain_water_level_sensor_read_raw_fake.return_val = 200;
    watering_service_water_pot();
    TEST_ASSERT_EQUAL_UINT32(1, rain_water_level_sensor_read_raw_fake.call_count);
    TEST_ASSERT_EQUAL_UINT32(1, pump_controller_init_fake.call_count);
    TEST_ASSERT_EQUAL_UINT32(1, activate_pump_fake.call_count);
    TEST_ASSERT_EQUAL_UINT32(750, activate_pump_fake.arg0_val);
}
