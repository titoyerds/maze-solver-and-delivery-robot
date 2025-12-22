#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "gpio.h"
#include "sensor.h"

#define TRIG_PORT GPIO_PORT_C
#define TRIG_PIN 0
#define ECHO_PORT GPIO_PORT_B
#define ECHO_PIN 0

#define PAYLOAD_PORT GPIO_PORT_C
#define PAYLOAD_PIN 2

volatile uint8_t payload_change_detected = 0;

// static global variables
static uint16_t pulse_width = 0;
static uint8_t measurement_complete = 0;

// TIMER1_CAPT_vect ISR
ISR(TIMER1_CAPT_vect) {
    // Implement state machine logic
    // A static variable is used to remember if waiting for a rising or falling edge.
    // If rising edge: store ICR1 in start_time, reconfigure ICU for falling edge.
    // If falling edge: pulse_width = ICR1 - start_time, set flag, reconfigure ICU for rising edge.
    static uint8_t edge_wait = 1;
    static uint16_t start_time = 0;

    if (edge_wait) {    // rising edge
        start_time = ICR1;
        TCCR1B &= ~(1 << ICES1);
        edge_wait = 0;
    } else {
        pulse_width = ICR1 - start_time;
        measurement_complete = 1;
        TCCR1B |= (1 << ICES1);
        edge_wait = 1;
    }
}

// sensor_init() implementation
void sensor_init(void) {
    // Use GPIO driver to set Trig pin as output and Echo (PB0) as input.
    gpio_set_direction(TRIG_PORT, TRIG_PIN, GPIO_PIN_OUTPUT);
    gpio_set_direction(ECHO_PORT, ECHO_PIN, GPIO_PIN_INPUT);
    gpio_set_direction(PAYLOAD_PORT, PAYLOAD_PIN, GPIO_PIN_INPUT);

    gpio_set_pullup(PAYLOAD_PORT, PAYLOAD_PIN, 1);
    // Timer1 Configuration for ICU
    // A tick rate is needed that won't overflow the 16-bit timer for the max sensor range.
    // Max range ~400cm => ~23ms echo
    // A prescaler of 8 gives a 0.5µs tick and max time of ~32ms. Perfect.
    // Timer1 prescaler set to 8 (CS11 bit in TCCR1B).
    TCCR1B |= (1 << CS11);

    // Enable the Input Capture Interrupt (ICIE1 bit in TIMSK1).
    TIMSK1 |= (1 << ICIE1);

    // Configure the ICU to trigger on a RISING edge initially (ICES1 bit in TCCR1B).
    TCCR1B |= (1 << ICES1);

    // Enable Pin Change Interrupts for PORT C Group (PCINT1_vect)
    // PCIE1 bit enables interrupts for PC0-PC6
    PCICR |= (1 << PCIE1);

    // Unmask (Enable) specifically PC2
    PCMSK1 |= (1 << PCINT10);
}

// Implement all public API functions.
void sensor_trigger(void) {
    measurement_complete = 0;
    TCNT1 = 0;

    gpio_write(TRIG_PORT, TRIG_PIN, GPIO_PIN_HIGH);
    _delay_us(10);
    gpio_write(TRIG_PORT, TRIG_PIN, GPIO_PIN_LOW);
}
uint8_t sensor_is_measurement_complete(void) {
    return measurement_complete;
}
uint16_t sensor_get_pulse_width(void) {
    measurement_complete = 0;
    return pulse_width;
}

// Reads the Payload IR Sensor on PC2
// Returns 1 if Payload Detected (Active Low), 0 if Empty
uint8_t sensor_read_payload(void) {
    // Assuming PC2. Check if bit is LOW (0).
    // Note: Ensure you initialized this pin as INPUT_PULLUP in sensor_init()
    if (gpio_read(PAYLOAD_PORT, PAYLOAD_PIN) == GPIO_PIN_LOW ) {
        return 1; // Detected
    }
    return 0; // Empty
}

// --- INTERRUPT SERVICE ROUTINE ---
ISR(PCINT1_vect) {
    // Triggers instantly on ANY change (Empty->Full OR Full->Empty)
    payload_change_detected = 1; 
}