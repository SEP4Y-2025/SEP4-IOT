#ifndef MOCK_LIGHT_H
#define MOCK_LIGHT_H

#include <stdint.h>

void light_init(void);
uint16_t light_read(void);

extern uint16_t mock_light_adc_value;

#endif
