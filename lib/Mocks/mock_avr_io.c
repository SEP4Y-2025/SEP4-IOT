#include "mock_avr_io.h"

// instantiate every extern’d register/variable
uint8_t UCSR0C;
uint8_t UCSR1C;
uint8_t UCSR2C;
uint8_t UCSR3C;

uint8_t UCSR0B;
uint8_t UCSR1B;
uint8_t UCSR2B;
uint8_t UCSR3B;

uint8_t UCSR0A;
uint8_t UCSR1A;
uint8_t UCSR2A;
uint8_t UCSR3A;

uint8_t UDR0;
uint8_t UDR1;
uint8_t UDR2;
uint8_t UDR3;

uint8_t UBRR0H;
uint8_t UBRR0L;
uint8_t UBRR1H;
uint8_t UBRR1L;
uint8_t UBRR2H;
uint8_t UBRR2L;
uint8_t UBRR3H;
uint8_t UBRR3L;

uint8_t DDRB;
uint8_t PORTB;
uint8_t DDRA;
uint8_t DDRC;
uint8_t PORTC;
uint8_t PORTA;
uint8_t DDRG;
uint8_t PORTG;
uint8_t DDRD;
uint8_t PORTD;
uint8_t PIND;
uint8_t PINC;
uint8_t DDRF;
uint8_t PORTF;
uint8_t PINF;
uint8_t PINL;
uint8_t DDRL;
uint8_t PORTL;
uint8_t PINB;
uint8_t EIMSK;
uint8_t EICRA;
uint8_t DDRE;
uint8_t PORTE;
uint8_t DDRH;
uint8_t TCCR1B;
uint16_t OCR1A;
uint8_t TIMSK1;
uint8_t PORTH;
uint8_t DDRK;
uint8_t PORTK;
uint8_t TCNT1;
uint8_t PINK;
uint8_t ADMUX;
uint8_t ADCSRA;
uint8_t ADCSRB;
uint8_t ADCL;
uint8_t ADCH;
uint8_t DIDR2;
uint8_t TCCR3A;
uint8_t TCCR3B;
uint8_t OCR3A;
uint8_t TIMSK3;
uint8_t OCR3B;
uint8_t OCR3C;
uint16_t ADC; // your 16-bit ADC register
// …and any others you declared extern…

uint8_t UCSR0C;
// … all your other uint8_t and uint16_t registers …

// --- Delay and interrupt stubs ---
void _delay_ms(int ms) { (void)ms; }
void _delay_us(int us) { (void)us; }

void cli(void) { /* interrupts disabled (no-op) */ }
void sei(void) { /* interrupts enabled  (no-op) */ }