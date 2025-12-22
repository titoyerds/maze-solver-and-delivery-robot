#include <avr/io.h>
#include <stdio.h>
#include "uart.h"

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VAL (F_CPU / (16UL * BAUD)) - 1

// static uart_putchar() function
static int uart_putchar(char c, FILE *stream) {
    while ((UCSR0A & (1 << UDRE0)) == 0) {
        ;
    }
    UDR0 = c;

    return 0;
}

// static FILE stream object
static FILE uart_stream = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

// uart_init() implementation
void uart_init(void) {
    // Set the baud rate in UBRR0H and UBRR0L
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)(UBRR_VAL);

    // Enable transmitter (TXEN0) in UCSR0B
    // Enable the receiver (`RXEN0`) and the Receive Complete Interrupt (`RXCIE0`) in the `UCSR0B` register.
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

    // Set the frame format to 8 data, 1 stop bit (UCSZ01, UCSZ00 in UCSR0C).
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Implement uart_get_stream()
FILE *uart_get_stream(void) {
    return &uart_stream;
}