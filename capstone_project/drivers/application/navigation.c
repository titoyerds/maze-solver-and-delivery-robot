#include "navigation.h"
#include "sensor.h"
#include "servo.h"
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define SERVO_LEFT   180
#define SERVO_FRONT   90
#define SERVO_RIGHT    0

volatile uint16_t ultrasonic_read_cm_blocking(void)
{
    uint16_t timeout = 0;

    sensor_trigger();
    while (!sensor_is_measurement_complete()) {
        _delay_us(10);
        if (++timeout > 3000) {
            return 400; // no echo
        }
    }
    return sensor_get_pulse_width() / 116;
}

void navigation_init(void)
{
    servo_init();
}

// Generate ~20ms pulse several times to let servo move
static void servo_hold(uint8_t angle, uint8_t cycles)
{
    servo_set_angle(angle);
    for (uint8_t i = 0; i < cycles; i++) {
        cli();
        servo_update();  // software PWM call
        sei();
    }
}

void navigation_scan(nav_scan_t *scan)
{
    printf("Looking...\n");
    // Move servo LEFT and measure
    servo_hold(SERVO_LEFT, 25);      // ~25×20ms = 500ms
    scan->left = ultrasonic_read_cm_blocking();

    // Move servo RIGHT and measure
    servo_hold(SERVO_RIGHT, 25);
    scan->right = ultrasonic_read_cm_blocking();

    // Move servo FRONT
    servo_hold(SERVO_FRONT, 5);
}