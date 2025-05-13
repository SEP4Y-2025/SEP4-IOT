#include "rain_water_level_sensor.h"
#include "Arduino.h"  // Include Arduino library for digitalRead and pinMode

#define RAIN_SENSOR_PIN 7  // Define the digital pin connected to the DO pin of the sensor

// Initializes the rain water level sensor by setting the pin mode
void rain_water_level_sensor_init(void) {
    pinMode(RAIN_SENSOR_PIN, INPUT);  // Set the sensor pin as input
}

// Reads the water level from the sensor
// Returns RAIN_SENSOR_OK if reading is successful, RAIN_SENSOR_FAIL otherwise
RAIN_SENSOR_ERROR_MESSAGE_t rain_water_level_sensor_read(uint8_t* water_level) {
    if (water_level == NULL) {
        return RAIN_SENSOR_FAIL;  // Return fail if the pointer is NULL
    }

    uint8_t level = digitalRead(RAIN_SENSOR_PIN);  // Read the sensor pin (HIGH or LOW)

    // If the reading is successful, return the water level and status
    if (level == HIGH) {
        *water_level = 1;  // Water level is above the threshold
    } else {
        *water_level = 0;  // Water level is below the threshold
    }

    return RAIN_SENSOR_OK;
}
