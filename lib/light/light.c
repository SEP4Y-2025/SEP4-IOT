#include "avr/io.h"

// ==== ADC FUNCTIONS ====

void adc_init(void) {
    // Reference = AVcc
    ADMUX = (1 << REFS0);
    
    // ADC enable, prescaler = 128
    ADCSRA = (1 << ADEN)  | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);  // Select ADC channel

    ADCSRA |= (1 << ADSC);  // Start conversion

    while (ADCSRA & (1 << ADSC));  // Wait for conversion complete

    return ADC;  // Return ADC value
}
