#pragma once
#include <stdint.h>

void cli(void);
void sei(void);
#define PB7 7
#define F_CPU 16000000L
#define TXEN0 3
#define RXEN0 4
#define RXCIE0 7

#define DDA6 6
#define DDA4 4
#define DDC7 7
#define DDC1 1
#define PG1 1

#define PD7 7

#define TXEN1 3
#define RXEN1 4
#define RXCIE1 7

#define TXEN2 3
#define RXEN2 4

#define RXCIE2 7

#define TXEN3 3
#define RXEN3 4
#define RXCIE3 7

#define UCSZ00 1
#define UCSZ01 2
extern uint8_t UCSR0C;

#define UCSZ10 1
#define UCSZ11 2
extern uint8_t UCSR1C;

#define UCSZ20 1
#define UCSZ21 2
extern uint8_t UCSR2C;

#define UCSZ30 1
#define UCSZ31 2
#define PD1 1

extern uint8_t UCSR3C;

extern uint8_t UCSR1B;
#define UDRIE1 5
extern uint8_t UCSR2B;
#define UDRIE2 5
extern uint8_t UCSR3B;
#define UDRIE3 5
extern uint8_t UCSR0B;
#define UDRIE0 5
extern uint8_t UCSR0A;
extern uint8_t UCSR1A;
extern uint8_t UCSR2A;
extern uint8_t UCSR3A;

#define UDRE0 5
#define UDRE1 5
#define UDRE2 5
#define UDRE3 5
void _delay_ms(int a);
void _delay_us(int a);
extern uint8_t DDRB;
extern uint8_t PORTB;

extern uint8_t UDR0;
extern uint8_t UCSR0B;
extern uint8_t UBRR0H;
extern uint8_t UBRR0L;

extern uint8_t UDR1;
extern uint8_t UCSR1B;
extern uint8_t UBRR1H;
extern uint8_t UBRR1L;

extern uint8_t UDR2;
extern uint8_t UCSR2B;
extern uint8_t UBRR2H;
extern uint8_t UBRR2L;

extern uint8_t UDR3;
extern uint8_t UCSR3B;
extern uint8_t UBRR3H;
extern uint8_t UBRR3L;

extern uint8_t DDRA;
extern uint8_t DDRC;

extern uint8_t PORTC;
extern uint8_t PORTA;

extern uint8_t DDRG;
extern uint8_t PORTG;

extern uint8_t DDRD;
extern uint8_t PORTD;
extern uint8_t PIND;
extern uint8_t PINC;

extern uint8_t DDRF;
extern uint8_t PORTF;
extern uint8_t PINF;
#define ISC21 5
#define ISC20 4
#define PA7 7
#define PL1 1
#define ADC15D 7
extern uint8_t PINL;
extern uint8_t DDRL;
extern uint8_t PORTL;
#define MUX0 0
#define MUX2 2
#define INT2 2
extern uint8_t PINB;
extern uint8_t EIMSK;
extern uint8_t EICRA;
#define PB1 1
#define PB2 2
#define PB3 3
#define PB0 0

#define PL6 6
#define PL7 7

#define PF1 1
#define PF2 2
#define PF3 3
#define PD0 0
#define PD1 1
#define PD2 2

extern uint8_t DDRE;
extern uint8_t PORTE;

#define PE5 5
#define PG5 5
#define PH5 5
#define PH4 4

extern uint8_t DDRH;
extern uint8_t TCCR1B;
extern uint16_t OCR1A;
extern uint8_t TIMSK1;
extern uint8_t PORTH;

#define WGM12 3
#define CS11 1
#define OCIE1A 1

extern uint8_t DDRK;
extern uint8_t PORTK;
extern uint8_t TCNT1;
extern uint8_t PINK;

#define PK6 6
#define PK5 5
#define PK7 7
#define PK4 4

#define CS12 2

#define PB6 6
#define PB5 5
#define PB4 4

extern uint8_t ADMUX;
extern uint8_t ADCSRA;
extern uint8_t ADCSRB;
extern uint8_t ADCL;
extern uint8_t ADCH;
extern uint8_t DIDR2;
#define REFS0 6
#define ADEN 7
#define ADPS2 2
#define ADPS1 1
#define ADPS0 0
#define MUX5 3
#define ADC9D 1
#define ADC8D 0
#define PK1 1
#define PK2 2
#define ADSC 6

extern uint8_t TCCR3A;
extern uint8_t TCCR3B;
extern uint8_t OCR3A;
extern uint8_t TIMSK3;
extern uint8_t OCR3B;
extern uint8_t OCR3C;

#define CS32 2
#define CS30 0
#define OCIE3A 1
#define OCIE3B 2
#define OCIE3C 3

extern uint8_t ADCSRA;
extern uint8_t ADCL;
extern uint8_t ADCH;
#define ADC10D 2
#define MUX1 1
#define REFS0 6
#define PK1 1
#define PK0 0

// hc-sr04
// Vcc
extern uint8_t DDRC;
extern uint8_t PORTC;
#define PC0 0

// GND
extern uint8_t DDRC;
#define PC6 6

// Trigger
extern uint8_t DDRC;
#define PC2 2
extern uint8_t PORTC;

// Echo
extern uint8_t PINC;
#define PC4 4

// pir
// Vcc

extern uint8_t PORTK;
#define PK5 5

// GND
extern uint8_t DDRK; // DDRK
#define PK3 3        // PK7

// signal

#define PK4 4

extern uint8_t PINK;

#define PCIE2 2
#define PCINT20 4
extern uint8_t PCICR;
extern uint8_t PCMSK2;

// tone
#define PA1 1
extern uint8_t TCCR2A;
extern uint8_t TCCR2B;
extern uint8_t TCNT2;

// Busy-wait
#define CS22 2
#define CS21 1
#define CS20 0

#define PE3 3
#define PC7 7
#define PA6 6
#define PA4 4

#define MUX3 3
#define MUX4 4
extern uint16_t ADC;

static inline uint8_t eeprom_read_byte(const uint8_t *addr)
{
        (void)addr;
        return 0x00;
}
static inline void eeprom_write_byte(uint8_t *addr, uint8_t val)
{
        (void)addr;
        (void)val;
}

static inline void eeprom_update_byte(uint8_t *addr, uint8_t val)
{
        *addr = val;
}

static inline void eeprom_update_dword(uint32_t *addr, uint32_t val)
{
        *addr = val;
}

static inline uint32_t eeprom_read_dword(const uint32_t *addr)
{
        return *addr;
}

static inline uint8_t digitalPinToPort(uint8_t pin)
{
        (void)pin;
        return 1; /* PORTB */
}
static inline uint8_t digitalPinToBitMask(uint8_t pin) { return (1 << pin); }

static inline volatile uint8_t *portModeRegister(uint8_t port)
{
        (void)port;
        return &PORTB;
}
static inline volatile uint8_t *portOutputRegister(uint8_t port)
{
        (void)port;
        return &PORTB;
}
static inline volatile uint8_t *portInputRegister(uint8_t port)
{
        (void)port;
        return &PINB;
}

#define NUM_DIGITAL_PINS 20
// stub the actual registers
extern uint8_t PORTB, PINB;

// Pin modes (already had these)
#define INPUT 0
#define OUTPUT 1

// Pin levels — add these!
#define LOW 0
#define HIGH 1

static inline void pinMode(uint8_t pin, uint8_t mode)
{
        (void)pin;
        (void)mode;
}
static inline void digitalWrite(uint8_t pin, uint8_t val)
{
        (void)pin;
        (void)val;
}
static inline int digitalRead(uint8_t pin)
{
        (void)pin;
        return 0;
}

typedef struct
{
        void (*begin)(unsigned long);
        int (*print)(const char *);
        int (*println)(const char *);
} MockSerial;

static void MockSerial_begin(unsigned long b) { (void)b; }
static int MockSerial_print(const char *s)
{
        (void)s;
        return 0;
}
static int MockSerial_println(const char *s)
{
        (void)s;
        return 0;
}

static MockSerial Serial = {
    .begin = MockSerial_begin,
    .print = MockSerial_print,
    .println = MockSerial_println};

// Analog pin numbers
#define A0 0
#define A1 1
#define A2 2
#define A3 3
#define A4 4
#define A5 5

static inline int analogRead(uint8_t pin)
{
        (void)pin;
        return 0; // or some test-driven value if you want
}
