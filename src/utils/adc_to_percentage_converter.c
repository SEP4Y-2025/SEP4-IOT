#include "adc_to_percentage_converter.h"
#include <stdint.h>

uint8_t convert_adc_to_percentage(uint8_t soil_humidity_adc_value)
{
    uint8_t soil_humidity_percentage = (soil_humidity_adc_value * 100) / 1023;
    if (soil_humidity_percentage > 100)
        soil_humidity_percentage = 100; // Cap the value at 100%
    else if (soil_humidity_percentage < 0)
        soil_humidity_percentage = 0; // Cap the value at 0%
        
    return soil_humidity_percentage;
}
