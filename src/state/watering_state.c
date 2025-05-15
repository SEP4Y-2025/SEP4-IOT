#include <avr/eeprom.h>
#include "watering_state.h"
#include <stdio.h>
#include "services/logger_service.h"

#define EEPROM_MARKER_ADDR ((uint8_t *)0)
#define EEPROM_FREQ_ADDR ((uint32_t *)1)
#define EEPROM_DOSAGE_ADDR ((uint32_t *)5)
#define EEPROM_TELEMETRY_ADDR ((uint8_t *)9)
#define EEPROM_WATERING_ADDR ((uint8_t *)10)
#define EEPROM_VALID_MARKER 0xAA

static uint32_t watering_frequency = 0; // default 0 hours
static uint32_t water_dosage = 0;       // default 0 ml
static bool telemetry_enabled = true;
static bool watering_enabled = false;

static void save_config()
{
    eeprom_update_dword(EEPROM_FREQ_ADDR, watering_frequency);
    eeprom_update_dword(EEPROM_DOSAGE_ADDR, water_dosage);
    eeprom_update_byte(EEPROM_TELEMETRY_ADDR, telemetry_enabled);
    eeprom_update_byte(EEPROM_WATERING_ADDR, watering_enabled);
    eeprom_update_byte(EEPROM_MARKER_ADDR, EEPROM_VALID_MARKER);
}

// Load from EEPROM (if valid), otherwise save defaults
void load_watering_state() {
    uint8_t marker = eeprom_read_byte(EEPROM_MARKER_ADDR);
    if (marker == EEPROM_VALID_MARKER)
    {
        watering_frequency = eeprom_read_dword(EEPROM_FREQ_ADDR);
        water_dosage = eeprom_read_dword(EEPROM_DOSAGE_ADDR);
        telemetry_enabled = eeprom_read_byte(EEPROM_TELEMETRY_ADDR);
        watering_enabled = eeprom_read_byte(EEPROM_WATERING_ADDR);
    }
    else
    {
        watering_frequency = 0;
        water_dosage = 0;
        telemetry_enabled = false;
        watering_enabled = false;
        save_config(); // store defaults if not valid
    }
}

void update_watering_settings(uint32_t freq, uint32_t dosage)
{
    if (watering_frequency != freq || water_dosage != dosage)
    {
        watering_frequency = freq;
        water_dosage = dosage;
        save_config();
    }
}

uint32_t get_watering_frequency()
{
    return watering_frequency;
}

uint32_t get_water_dosage()
{
    return water_dosage;
}

void log_stored_watering_state() {
    uint8_t marker = eeprom_read_byte(EEPROM_MARKER_ADDR);
    uint32_t freq = eeprom_read_dword(EEPROM_FREQ_ADDR);
    uint32_t dosage = eeprom_read_dword(EEPROM_DOSAGE_ADDR);
    uint8_t telem = eeprom_read_byte(EEPROM_TELEMETRY_ADDR);
    uint8_t water = eeprom_read_byte(EEPROM_WATERING_ADDR);

    char buffer[128];
    snprintf(buffer, sizeof(buffer),
             "EEPROM Marker: 0x%02X, Freq: %lu h, Dosage: %lu ml, Telemetry: %s, Watering: %s\n",
             marker, freq, dosage,
             telem ? "ON" : "OFF",
             water ? "ON" : "OFF");

    logger_service_log(buffer);
}

void set_telemetry_enabled(bool enabled)
{
    telemetry_enabled = enabled;
    eeprom_update_byte(EEPROM_TELEMETRY_ADDR, telemetry_enabled);
}

bool is_telemetry_enabled()
{
    return telemetry_enabled;
}

void set_watering_enabled(bool enabled)
{
    watering_enabled = enabled;
    eeprom_update_byte(EEPROM_WATERING_ADDR, watering_enabled);
}

bool is_watering_enabled()
{
    return watering_enabled;
}
