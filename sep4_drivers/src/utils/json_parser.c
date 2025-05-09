#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Call this with your payload string
bool parse_watering_payload(const char *payload, uint32_t *frequency, uint32_t *dosage) {
    const char *freq_key = "\"watering_frequency\":";
    const char *dosage_key = "\"water_dosage\":";

    const char *freq_pos = strstr(payload, freq_key);
    const char *dosage_pos = strstr(payload, dosage_key);

    if (!freq_pos || !dosage_pos) {
        return false;
    }

    // Move pointer to after the key
    freq_pos += strlen(freq_key);
    dosage_pos += strlen(dosage_key);

    // Extract numbers
    *frequency = (uint32_t)atoi(freq_pos);
    *dosage = (uint32_t)atoi(dosage_pos);

    return true;
}
