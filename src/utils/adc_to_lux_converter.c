#include "adc_to_lux_converter.h"
#include <stdint.h>

uint32_t convert_adc_to_lux(uint16_t light_adc_value)
{
    uint32_t lux_value = (light_adc_value * 10000) / 1023;
    if (lux_value > 10000)
    {
        lux_value = 10000; // Cap the value at 10000 lux
    }
    return lux_value;
}
