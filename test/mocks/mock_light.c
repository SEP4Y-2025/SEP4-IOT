// mock_light.c
#include "mock_light.h"

uint16_t mock_light_adc_value = 512;

void light_init(void) {
    // No-op
}

uint16_t light_read(void) {
    return mock_light_adc_value;
}
