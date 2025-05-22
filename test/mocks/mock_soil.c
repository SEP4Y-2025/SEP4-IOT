// mock_soil.c
#include "mock_soil.h"

uint16_t mock_soil_adc_value = 400;

void soil_init(void) {
    // No-op
}

uint16_t soil_read(void) {
    return mock_soil_adc_value;
}
