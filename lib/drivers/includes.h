#ifdef WINDOWS_TEST
#include "mock_avr_io.h"
#else
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <pins_arduino.h>
#include <Arduino.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
