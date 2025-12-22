#include "motor.h"
#include "gpio.h"

// Define all the motor and PWM pin connections
#define INA_PORT GPIO_PORT_D
#define INA_PIN 2
#define INB_PORT GPIO_PORT_D
#define INB_PIN 4

#define ENA_PORT GPIO_PORT_D
#define ENA_PIN 6
#define ENB_PORT GPIO_PORT_D
#define ENB_PIN 5

static uint8_t speed_var = 0;

void motor_init(void) {
    // configure all motor pins as outputs.
    gpio_set_direction(INA_PORT, INA_PIN, GPIO_PIN_OUTPUT);
    gpio_set_direction(INB_PORT, INB_PIN, GPIO_PIN_OUTPUT);

    // set PWM pins as outputs
    gpio_set_direction(ENA_PORT, ENA_PIN, GPIO_PIN_OUTPUT);
    gpio_set_direction(ENB_PORT, ENB_PIN, GPIO_PIN_OUTPUT);

    // Configure Timer0 for Fast PWM mode
    // Set WGM01 and WGM00 for Fast PWM.
    TCCR0A |= (1 << WGM01) | (1 << WGM00);

    // Set COM0A1 and COM0B1 for non-inverting output on OC0A and OC0B.
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);

    // Set prescaler (e.g., 64) using CS01 and CS00 bits in TCCR0B.
    TCCR0B |= (1 << CS01) | (1 << CS00);

    // Set initial speed (duty cycle) to 0. OCR0A/B can be 0-255.
    OCR0A = 0; // Left motor speed
    OCR0B = 0; // Right motor speed
}

void motor_set_speed(uint8_t speed) {
    speed_var = speed;
    OCR0A = speed;
    OCR0B = speed;
}
void motor_forward(void) {
    // Use gpio_write to set
    // INA=H, INB=L
    gpio_write(INA_PORT, INA_PIN, GPIO_PIN_LOW);
    gpio_write(INB_PORT, INB_PIN, GPIO_PIN_LOW);

    OCR0A = speed_var;
    OCR0B = speed_var;
}
void motor_backward(void) {
    // INA=L, INB=H
    gpio_write(INA_PORT, INA_PIN, GPIO_PIN_HIGH);
    gpio_write(INB_PORT, INB_PIN, GPIO_PIN_HIGH);
    OCR0A = speed_var;
    OCR0B = speed_var;
}
void motor_turn_left(void) {
    // INA=H (backwards turn left)
    gpio_write(INA_PORT, INA_PIN, GPIO_PIN_HIGH);
    OCR0A = speed_var;
    OCR0B = 0;
}
void motor_turn_right(void) {
    // INB=H (backwards turn right)
    gpio_write(INB_PORT, INB_PIN, GPIO_PIN_HIGH);
    OCR0A = 0;
    OCR0B = speed_var;
}
void motor_stop(void) {
    OCR0A = 0;
    OCR0B = 0;
}