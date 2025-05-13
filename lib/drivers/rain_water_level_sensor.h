#ifndef RAIN_WATER_LEVEL_SENSOR_H
#define RAIN_WATER_LEVEL_SENSOR_H

#include <stdint.h>

/**
 * @brief Rain Water Level Sensor Error messages.
 */
typedef enum{
    RAIN_SENSOR_OK,  /**< Sensor reading successful. */
    RAIN_SENSOR_FAIL /**< General failure or sensor operation unsuccessful. */
} RAIN_SENSOR_ERROR_MESSAGE_t;

/**
 * @brief Initializes the rain water level sensor.
 */
void rain_water_level_sensor_init(void);

/**
 * @brief Reads the water level from the sensor.
 * 
 * This function returns the water level status. The return value is:
 * - 1 if water is above the threshold (HIGH signal)
 * - 0 if water is below the threshold (LOW signal)
 * 
 * @return RAIN_SENSOR_ERROR_MESSAGE_t Status of the sensor reading (RAIN_SENSOR_OK or RAIN_SENSOR_FAIL).
 */
RAIN_SENSOR_ERROR_MESSAGE_t rain_water_level_sensor_read(uint8_t* water_level);

#endif
