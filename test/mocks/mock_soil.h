#ifndef MOCK_SOIL_H
#define MOCK_SOIL_H

#include <stdint.h>

void soil_init(void);
uint16_t soil_read(void);

extern uint16_t mock_soil_adc_value;

#endif
