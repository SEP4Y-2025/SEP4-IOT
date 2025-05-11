#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stdint.h>
#include <stdbool.h>

bool parse_watering_payload(const char *payload, uint32_t *frequency, uint32_t *dosage);

#endif 

