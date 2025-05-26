#include "pump_driver.h"
// #include <Arduino.h> // For digitalWrite and delay

#include <includes.h>

// Define the pin connected to the pump
#define PUMP_PIN 30

void pump_driver_init(void)
{
    pinMode(PUMP_PIN, OUTPUT);   // Set the pump pin as an output
    digitalWrite(PUMP_PIN, LOW); // Ensure the pump is off initially
}

void pump_driver_on(void)
{
    // logger_service_log("Pump ON");
    digitalWrite(PUMP_PIN, HIGH); // Turn the pump on
}

void pump_driver_off(void)
{
    // logger_service_log("Pump OFF");
    digitalWrite(PUMP_PIN, LOW); // Turn the pump off
}
void pump_driver_activate(uint32_t duration_ms)
{
    // logger_service_log("Activating pump for %lu ms", duration_ms);
    pump_driver_on();       // Turn the pump on
    _delay_ms(duration_ms); // Wait for 1 second to ensure the pump is on before starting the timer

    pump_driver_off(); // Turn the pump off after the delay
}