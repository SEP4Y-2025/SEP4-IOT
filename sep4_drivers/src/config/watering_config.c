#include <avr/eeprom.h>
#include "watering_config.h"

#define EEPROM_MARKER_ADDR     ((uint8_t*)0)
#define EEPROM_FREQ_ADDR       ((uint32_t*)1)
#define EEPROM_DOSAGE_ADDR     ((uint32_t*)5)
#define EEPROM_VALID_MARKER    0xAA

static uint32_t watering_frequency = 0; // default 0 hours
static uint32_t water_dosage = 0;      // default 0 mL

static void save_config() {
    eeprom_update_dword(EEPROM_FREQ_ADDR, watering_frequency);
    eeprom_update_dword(EEPROM_DOSAGE_ADDR, water_dosage);
    eeprom_update_byte(EEPROM_MARKER_ADDR, EEPROM_VALID_MARKER);
}

// Load from EEPROM (if valid), otherwise save defaults
void load_watering_config() {
    uint8_t marker = eeprom_read_byte(EEPROM_MARKER_ADDR);
    if (marker == EEPROM_VALID_MARKER) {
        watering_frequency = eeprom_read_dword(EEPROM_FREQ_ADDR);
        water_dosage = eeprom_read_dword(EEPROM_DOSAGE_ADDR);
    } else {
        watering_frequency = 0;
        water_dosage = 0;
        save_config();  // store defaults if not valid
    }
}

void update_watering_settings(uint32_t freq, uint32_t dosage) {
    if (watering_frequency != freq || water_dosage != dosage) {
        watering_frequency = freq;
        water_dosage = dosage;
        save_config();
    }
}

uint32_t get_watering_frequency() {
    return watering_frequency;
}

uint32_t get_water_dosage() {
    return water_dosage;
}

void log_stored_watering_config() {
    uint8_t marker = eeprom_read_byte((uint8_t*)0);
    uint32_t freq = eeprom_read_dword((uint32_t*)1);
    uint32_t dosage = eeprom_read_dword((uint32_t*)5);

    char buffer[64];
    snprintf(buffer, sizeof(buffer), 
        "EEPROM Marker: 0x%02X, Frequency: %lu h, Dosage: %lu ml\n",
        marker, freq, dosage);
    
    logger_service_log(buffer);  // Or use Serial.print if not using logger
}
