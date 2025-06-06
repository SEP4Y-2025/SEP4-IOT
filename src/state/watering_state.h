#ifndef WATERING_STATE_H
#define WATERING_STATE_H

#include <stdint.h>
#include <stdbool.h>

void load_watering_state();
void update_watering_settings(uint32_t freq, uint32_t dosage);

uint32_t get_watering_frequency();
uint32_t get_water_dosage();
void log_stored_watering_state();

void set_telemetry_enabled(bool enabled);
bool is_telemetry_enabled(void);

void set_watering_enabled(bool enabled);
bool is_watering_enabled(void);

#endif
