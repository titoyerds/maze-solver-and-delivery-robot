#include <avr/io.h>
#include <avr/interrupt.h>
#include "gpio.h"
#include "servo.h"
#include <util/delay.h>

#define SERVO_PORT GPIO_PORT_B
#define SERVO_PIN  2

// Servo pulse width limits in microseconds
#define SERVO_MIN_US 500   // 0°
#define SERVO_MAX_US 2500   // 180°
#define SERVO_PERIOD_US 40000 // 40 ms

static uint16_t current_pulse_us = 1500; // center 90°

static uint16_t angle_to_us(uint8_t angle) {
    if (angle > 180) angle = 180;
    return SERVO_MIN_US + ((uint32_t)angle * (SERVO_MAX_US - SERVO_MIN_US)) / 180;
}

void servo_init(void) {
    gpio_set_direction(SERVO_PORT, SERVO_PIN, GPIO_PIN_OUTPUT);
    current_pulse_us = 1500; // center
}

void servo_set_angle(uint8_t angle) {
    current_pulse_us = angle_to_us(angle);
}

// Call this in your navigation_scan loop
void servo_update(void) {
    // Generate 20 ms pulse
    gpio_write(SERVO_PORT, SERVO_PIN, GPIO_PIN_HIGH);
    _delay_us(current_pulse_us);
    gpio_write(SERVO_PORT, SERVO_PIN, GPIO_PIN_LOW);
    _delay_us(SERVO_PERIOD_US - current_pulse_us);
}