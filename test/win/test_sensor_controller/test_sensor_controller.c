#include "unity.h"
#include "../fff.h"
#include "dht11.h"

// Include the controller under test
#include "controllers/sensor_controller.h"

DEFINE_FFF_GLOBALS;

// Declare fakes
FAKE_VALUE_FUNC(uint16_t, light_read);
FAKE_VALUE_FUNC(uint16_t, soil_read);
FAKE_VALUE_FUNC(uint16_t, rain_water_level_sensor_read_raw);
FAKE_VALUE_FUNC(uint8_t,  rain_water_level_sensor_read_percentage);
FAKE_VALUE_FUNC4(DHT11_ERROR_MESSAGE_t, dht11_get, uint8_t*, uint8_t*, uint8_t*, uint8_t*);

static DHT11_ERROR_MESSAGE_t fake_dht11_get(uint8_t *h_i, uint8_t *h_d, uint8_t *t_i, uint8_t *t_d) {
    *h_i = 55;
    *h_d = 1;
    *t_i = 22;
    *t_d = 5;
    return DHT11_OK;
}


void setUp(void) {
    RESET_FAKE(light_read);
    RESET_FAKE(soil_read);
    RESET_FAKE(rain_water_level_sensor_read_raw);
    RESET_FAKE(rain_water_level_sensor_read_percentage);
    RESET_FAKE(dht11_get);
}


void tearDown(void) {}

FAKE_VOID_FUNC(dht11_init);
FAKE_VOID_FUNC(light_init);
FAKE_VOID_FUNC(soil_init);
FAKE_VOID_FUNC(rain_water_level_sensor_init);

extern void sensor_controller_init(void);

void test_sensor_controller_init_should_call_all_sensor_inits(void)
{
    sensor_controller_init();
    TEST_ASSERT_EQUAL(1, dht11_init_fake.call_count);
    TEST_ASSERT_EQUAL(1, light_init_fake.call_count);
    TEST_ASSERT_EQUAL(1, soil_init_fake.call_count);
    TEST_ASSERT_EQUAL(1, rain_water_level_sensor_init_fake.call_count);
}

void test_sensor_controller_read_should_store_sensor_values(void)
{
    // Arrange
    uint8_t hum_i = 55, hum_d = 1, temp_i = 22, temp_d = 5;

    dht11_get_fake.custom_fake = fake_dht11_get;


    light_read_fake.return_val = 1234;
    soil_read_fake.return_val = 567;

    // Act
    sensor_controller_read();

    // Assert
    TEST_ASSERT_EQUAL_UINT8(22, sensor_controller_get_temperature_integer());
    TEST_ASSERT_EQUAL_UINT8(5, sensor_controller_get_temperature_decimal());
    TEST_ASSERT_EQUAL_UINT8(55, sensor_controller_get_humidity_integer());
    TEST_ASSERT_EQUAL_UINT8(1, sensor_controller_get_humidity_decimal());
    TEST_ASSERT_EQUAL_UINT16(1234, sensor_controller_get_light());
    TEST_ASSERT_EQUAL_UINT16(567, sensor_controller_get_soil());
}


extern uint16_t sensor_controller_get_water_level_raw(void);
extern uint8_t sensor_controller_get_water_level_percentage(void);

void test_sensor_controller_get_water_level(void)
{
    rain_water_level_sensor_read_raw_fake.return_val = 999;
    rain_water_level_sensor_read_percentage_fake.return_val = 42;

    TEST_ASSERT_EQUAL_UINT16(999, sensor_controller_get_water_level_raw());
    TEST_ASSERT_EQUAL_UINT8(42, sensor_controller_get_water_level_percentage());
}

