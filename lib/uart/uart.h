#include <avr/io.h>
void uartInit(int baudrate);
void uartSend(char *data);
char* uartReceive();